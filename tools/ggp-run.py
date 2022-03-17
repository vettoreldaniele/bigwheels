#!/usr/bin/env python3
"""GGP driver for BigWheels binaries.

This driver uploads a BigWheels project binary and its assets to a Stadia
instance.  Once uploaded, it executes it using 'ggp run'.

Example use,

$ tools/ggp-run.py bazel-bin/projects/20_camera_motion/20_camera_motion
"""

import argparse
import logging
import os
import subprocess
import sys
import ggp_utils

# Default location for the 'ggp' binary.
_GGP_BIN = 'ggp'


def _RunOnInstance(ggp_bin, instance, app_path, binary, binary_args, ggp_vars):
  """Runs the given binary on the instance.

  Args:
    ggp_bin: Path to the ggp executable.
    instance: The instance name or ID. None, if only one instance is reserved.
    app_path: Path relative to /mnt/developer where the binary can be found.
    binary: The binary to execute.
    binary_args: The command-line arguments to pass to the binary.
    ggp_vars: The --vars string to pass to the ggp binary.

  Returns:
    A return code value. 0 means success.
  """

  binary_cmd = '%s/%s %s' % (app_path, os.path.basename(binary), binary_args)

  cmd = [
      ggp_bin, 'run', '--no-launch-browser',
      '--application=Yeti Development Application', '--cmd', binary_cmd
  ]
  if instance is not None:
    cmd.extend(['--instance', instance])
  if ggp_vars:
    cmd.extend(['--vars', ggp_vars])
  logging.info('$ %s', ' '.join(cmd))
  return subprocess.call(cmd)


def main():
  logging.basicConfig(
      format='%(asctime)s %(module)s: %(message)s', level=logging.INFO)

  parser = argparse.ArgumentParser(
      description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument(
      '--instance',
      type=str,
      default=None,
      help='The instance name or ID to set up. This instance must be already '
      'reserved.  If you only have a single instance reserved, this can be '
      'empty (default: %(default)s).',
  )
  parser.add_argument(
      '--ggp_bin',
      default=_GGP_BIN,
      help='Path to the "ggp" binary to use (default: %(default)s).',
  )
  parser.add_argument(
      '--app_path',
      default='bw',
      help='Path relative to /mnt/developer where to store and execute '
      'the application (default: %(default)s).',
  )
  parser.add_argument(
      'binary',
      help='Binary to execute on the instance.',
  )
  parser.add_argument(
      '--binary_args',
      default='',
      help='The command-line arguments to pass to the binary.',
  )
  parser.add_argument(
      '--vars',
      default='',
      help='The --vars string to pass to the ggp binary as part of the `ggp run` command.',
  )
  args = parser.parse_args()

  sources = ['assets', args.binary]
  if os.path.exists('bazel-bin/assets'):
    sources.append('bazel-bin/assets')
  else:
    logging.warning(
        'Directory bazel-bin/assets not found, it will not be uploaded')
  rc = ggp_utils.SyncFilesToInstance(args.ggp_bin, args.instance, sources,
                                     '/mnt/developer/%s/' % args.app_path)
  if rc != 0:
    return rc

  return ggp_utils.RunOnInstance(args.ggp_bin, args.instance, args.app_path,
                                 args.binary, args.binary_args, args.vars)


if __name__ == '__main__':
  sys.exit(main())
