#! /usr/bin/env python

import os
import subprocess
import sys
import tempfile
import shutil
import crc

try:
    from argparse import ArgumentParser as Parser
except ImportError:
    from optparse import OptionParser as Parser

