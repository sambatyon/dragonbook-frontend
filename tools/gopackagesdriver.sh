#!/usr/bin/env bash
GOPACKAGESDRIVER_BAZEL_BUILD_FLAGS=--strategy=GoStdlibList=local \
exec bazel run -- @rules_go//go/tools/gopackagesdriver "${@}"
