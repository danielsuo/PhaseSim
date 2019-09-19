import os
import sys
import shutil
import logging
import signal
import subprocess
import itertools
import tqdm
import datetime

import click
import clickutil

import multiprocessing as mp

from ..parse import get_detectors
from ..constants import PHASESIM_HOME

timestamp = datetime.datetime.now().strftime("%Y-%m-%dT%H-%M-%S")

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("phasesim")


def job(cmd):
    output_dir = cmd[-1]
    output_log = os.path.join(cmd[-1], "log.out")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    print(output_dir)
    file = open(output_log, "w")
    proc = subprocess.Popen(cmd, stdout=file)
    proc.wait()


def run(traces, args, trace_dir, output_dir, num_cores, dry_run):
    if traces is None:
        find = subprocess.Popen(
            ["find", trace_dir, "-type", "f"], stdout=subprocess.PIPE)
        grep = subprocess.check_output(["grep", "xz"], stdin=find.stdout)
        traces = grep.decode("ascii").split()
    else:
        traces = traces.split(",")

    phasesim = os.path.join(PHASESIM_HOME, "build/phasesim")

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    cmds = []

    for trace in traces:

        name = ".".join(os.path.basename(trace).split(".")[:-2])
        output = os.path.join(output_dir, "{}.out".format(name))

        cmd = [phasesim, "-t", trace, "-o", os.path.join(output_dir, name)]

        job_args = {"benchmark": name}

        cmd.extend(args.format(**job_args).split())
        print(cmd)
        cmds.append(cmd)

    if dry_run:
        return

    with mp.Pool(processes=num_cores) as pool:
        result = list(tqdm.tqdm(pool.imap(job, cmds), total=len(cmds)))


@click.command(context_settings=dict(help_option_names=["-h", "--help"]))
@click.option("--traces", default=None, type=str)
@click.option(
    "-a", "--args", type=str, default="", help="Arguments to pass to phasesim")
@click.option(
    "-t",
    "--trace-dir",
    type=click.Path(exists=True),
    default="traces",
    help="Trace directory")
@click.option(
    "-o",
    "--output-dir",
    type=str,
    default=os.path.join(PHASESIM_HOME, "tmp", timestamp),
    help="Output directory")
@click.option(
    "--num-cores",
    type=int,
    default=mp.cpu_count(),
    help="Number of CPUs to use")
@click.option("--dry-run", type=bool, is_flag=True, help="don't run")
@clickutil.call(run)
def _run():
    """
    Runs a simulation given a traces file and the desired policies.

    TODO: Attempt to build a binary with the desired policies if it does
    not exist.

    TODO: Throw an error if a binary does not exist and cannot be built.
    """
    pass


# if __name__ == "__main__":
# _run()
