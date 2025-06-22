#!/bin/bash

# Restore original PATH if backed up
if [ -n "$OLD_PATH" ]; then
  export PATH="$OLD_PATH"
  unset OLD_PATH
  echo "Environment deactivated."
fi
