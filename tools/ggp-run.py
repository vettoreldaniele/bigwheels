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

# Default location for the 'ggp' binary.
_GGP_BIN = 'ggp'


def _SyncToInstance(ggp_bin, instance, sources, dst):
  """Transfers the given file to the given instance.

  Args:
    ggp_bin: Path to the ggp executable.
    instance: The instance name or ID. None, if only one instance is reserved.
    sources: A list of local paths to synchronize.
    dst: The path of the destination directory on the instance.

  Returns:
    A return code value. 0 means success.
  """
  cmd = [ggp_bin, 'ssh', 'sync', '-r', '--progress']
  if instance is not None:
    cmd.extend(['--instance', instance])
  cmd.extend(sources)
  cmd.append(dst)
  logging.info('$ %s', ' '.join(cmd))
  return subprocess.call(cmd)


def _RunOnInstance(ggp_bin, instance, app_path, binary):
  """Runs the given binary on the instance.

  Args:
    ggp_bin: Path to the ggp executable.
    instance: The instance name or ID. None, if only one instance is reserved.
    app_path: Path relative to /mnt/developer where the binary can be found.
    binary: The binary to execute.

  Returns:
    A return code value. 0 means success.
  """
  cmd = [
      ggp_bin, 'run', '--no-launch-browser',
      '--application=Yeti Development Application', '--cmd',
      '%s/%s' % (app_path, os.path.basename(binary))
  ]
  if instance is not None:
    cmd.extend(['--instance', instance])
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
  args = parser.parse_args()

  sources = ['assets', args.binary]
  if os.path.exists('bazel-bin/assets'):
    sources.append('bazel-bin/assets')
  else:
    logging.warning(
        'Directory bazel-bin/assets not found, it will not be uploaded')
  rc = _SyncToInstance(args.ggp_bin, args.instance, sources,
                       '/mnt/developer/%s/' % args.app_path)
  if rc != 0:
    return rc

  return _RunOnInstance(args.ggp_bin, args.instance, args.app_path, args.binary)


if __name__ == '__main__':
  sys.exit(main())
