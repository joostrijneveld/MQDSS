#!/bin/sh

echo "Running all available tests.."
find test/ -name 'test_*' -type f -executable -exec {} \; | grep --color=auto -E "FAIL|$"
echo "Done!"
