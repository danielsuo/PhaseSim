import os
import re
import struct
import numpy as np
import pandas as pd
import seaborn as sns
import subprocess
import tqdm
import subprocess

from .constants import PHASESIM_HOME

headline = ("CPU \d "
            "cumulative IPC: (\d+\.\d+) "
            "instructions: (\d+) "
            "cycles: (\d+)")
headline = re.compile(headline)

heartbeat = ("CPU \d "
             "instructions: (\d+) "
             "cycles: (\d+) "
             "heartbeat IPC: (\d+\.\d+) "
             "cumulative IPC: (\d+\.\d+) "
             "\(Simulation time: (\d+) hr (\d+) min (\d+) sec\)")
heartbeat = re.compile(heartbeat)

def check(directory, date):
    df = process("{}/{}".format(date, directory),
                 "{}/{}/leela.simpoint.block.opt".format(date, directory))
    df = df[df.rref < 1e15]
    return df


def parse_headline(path):
    file = open(path, "r")
    for line in file:
        if line.find("CPU") != 0:
            continue
        groups = headline.search(line)
        if groups is None:
            continue
        file.close()

        ipc, instructions, cycles = groups.groups()
        ipc = float(ipc)
        instructions = int(instructions)
        cycles = int(cycles)
        return ipc, instructions, cycles

    return 0, 0, 0


def parse_counters(path):
    file = open(path, "r")
    results = {}
    for line in file:
        if line.find(": ") == -1:
            continue
        data = line.strip().split(": ")
        if len(data) != 2:
            continue

        try:
            results[data[0]] = int(data[1].strip())
        except Exception as e:
            continue

    return results


def parse_heartbeats(path):
    file = open(path, "r")

    results = {}
    counter = 0
    for line in file:
        if line.find("Heartbeat") != 0:
            continue

        groups = heartbeat.search(line)

        if groups is None:
            continue

        results[counter] = groups.groups()

        counter += 1
        if counter % 100 == 0:
            print("Processed {} heartbeats".format(counter), end="\r")

    file.close()

    df = pd.DataFrame.from_dict(
        results,
        orient="index",
        columns= [
            "instructions", "cycles", "heartbeat_ipc", "cumulative_ipc", "hours", "minutes", "seconds"
        ])

    df = df.astype({
        "instructions": "uint64",
        "cycles": "uint64",
        "heartbeat_ipc": "float64",
        "cumulative_ipc": "float64",
        "hours": "uint64",
        "minutes": "uint64",
        "seconds": "uint64"
    })

    return df


def parse_ipc_stats(path):
    fmt = "QQ"
    size = struct.calcsize(fmt)
    instructions = []
    cycles = []
    dinstructions = [0]
    dcycles = [0]
    with open(path, "rb") as f:
        counter = 0
        while True:
            data = f.read(size)
            if not data:
                break

            instruction, cycle = struct.unpack(fmt, data)
            instructions.append(int(instruction))
            cycles.append(int(cycle))

            if counter > 0:
                dinstructions.append(instructions[-1] - instructions[-2])
                dcycles.append(cycles[-1] - cycles[-2])

            counter += 1
            if counter % 10000 == 0:
                print("Processed {} instructions".format(instructions[-1]), end="\r")

    df = pd.DataFrame({
        "instructions": instructions,
        "cycles": cycles,
        "dinstructions": dinstructions,
        "dcycles": dcycles
    })

    df["ipc"] = df.dinstructions / df.dcycles

    return df


def parse_telemetry(path,
                    header,
                    columns=None,
                    types=None,
                    max_lines=None,
                    max_entries=None):
    file = open(path, "r")

    data = []
    lines = 0
    entries = 0

    if columns is None:
        ncol = 0
    else:
        ncol = len(columns)

    for line in file:
        lines += 1
        if (max_lines is not None
                and lines > max_lines) or (max_entries is not None
                                           and entries > max_entries):
            break
        if lines % 100000 == 0:
            print("Parsed {} entries and {} lines".format(entries, lines), end="\r")
        if line.find(header) != 0:
            continue

        new = line.split()[1:]

        if ncol == 0:
            ncol = len(new)
            columns = [str(x) for x in range(ncol)]
        if len(new) != ncol:
            continue
        data.append(new)

        entries += 1

    print("Found {} entries".format(entries))

    df = pd.DataFrame(data, columns=columns)

    if types is not None:
        if type(types) == str:
            types = [types] * len(columns)
        elif len(types) != len(columns):
            assert (0)
        astype = {}
        for i in range(len(types)):
            astype[columns[i]] = types[i]

        df = df.astype(astype)
        return df

    astype = {}
    for column in columns:
        try:
            val = float(df.iloc[-1][column])
            # if val % 1 == 0:
                # astype[column] = "int64"
            # else:
            astype[column] = "float64"
        except (ValueError, IndexError):
            pass

    df = df.astype(astype)

    return df


def get_detectors():
    find = subprocess.Popen(["find", PHASESIM_HOME, "-type", "f"], stdout=subprocess.PIPE)
    grep = subprocess.check_output(["grep", "PhaseDetector"], stdin=find.stdout)

    phase_detectors = [os.path.basename(detector) for detector in grep.decode("ascii").split()]
    phase_detectors = [detector.replace(".h", "") for detector in phase_detectors if detector != "PhaseDetector.h"]


def parse_phases(path, output_df=False):
    detectors = get_detectors()

    dfs = {}
    for detector in detectors:
        print("Parsing {}".format(detector))
        dfs[detector] = parse_telemetry(path, detector).iloc[:,0]

    return pd.DataFrame.from_records(dfs) if output_df else dfs
