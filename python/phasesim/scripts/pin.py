import os
import subprocess
import sys

import click
import clickutil

from ..constants import PHASESIM_HOME


def pin(
        tool,
        tool_args,
        executable,
        build_dir,
        output_dir,
        output_file,
        args,
):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Find PIN binary
    pin_binary = os.path.join(PHASESIM_HOME, build_dir,
                              "deps/pin/pin-git-prefix/src/pin-git/pin")
    pin_binary = os.path.relpath(pin_binary, os.getcwd())

    # Find tool binary
    tool_binary = os.path.join(PHASESIM_HOME, "ref/pintools", tool,
                               "obj-intel64", "{}.so".format(tool))
    tool_binary = os.path.relpath(tool_binary, os.getcwd())

    if os.path.realpath(build_dir) == os.path.dirname(
            os.path.realpath(executable)):
        build_dir = ""

    if not os.path.exists(pin_binary):
        raise Exception("Pin not installed. Please re-run CMake.")

    if not os.path.exists(tool_binary):
        raise Exception("Tool {} not found at path {}".format(
            tool, tool_binary))

    if not os.path.exists(executable):
        raise Exception("Binary {} not found".format(executable))

    cmd = [pin_binary, "-t", tool_binary]
    cmd.extend(tool_args.split())
    cmd.extend(["--", executable])
    cmd.extend(args)

    print("Running the following pintool:\n  {}".format(" ".join(cmd)))
    proc = subprocess.Popen(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out = proc.stdout.read()
    err = proc.stderr.read()
    proc.wait()

    # if tool == "tracer":
    # subprocess.check_call(["gzip", "-f", "champsim.trace"])
    # subprocess.check_call([
    # "mv", "champsim.trace.gz", "{}/{}.gz".format(
    # output_dir, output_file)
    # ])

    print(out.decode("ascii"))
    print(err.decode("ascii"))

    return out, err


@click.command(
    context_settings=dict(
        help_option_names=["-h", "--help"], ignore_unknown_options=True))
@click.option(
    "--tool", "-t", default="tracer", type=str, help="Which pintool to use")
@click.option(
    "--tool-args",
    "-a",
    default="",
    type=str,
    help="Arguments for the pintool")
@click.option(
    "--executable",
    "-e",
    default="/bin/ls",
    type=str,
    help="Which executable to profile")
@click.option(
    "--build-dir", "-b", default="build", type=str, help="Build directory")
@click.option(
    "--output-dir", "-d", default="traces", type=str, help="Output directory")
@click.option(
    "--output-file", "-o", default="result", type=str, help="Output results")
@click.argument("args", nargs=-1)
@clickutil.call(pin)
def _pin():
    """
    Runs the specified pin tool on the specified binary
    """


if __name__ == "__main__":
    _pin()
