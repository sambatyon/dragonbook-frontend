load("@rules_python//python:defs.bzl", "py_test")
load("@pydeps//:requirements.bzl", "requirement")

def pytest_test(name, srcs, deps = [], args = [], **kwargs):
    """
        Call pytest
    """
    py_test(
        name = name,
        srcs = [
            "//tools/python/pytest:pytest_wrapper.py",
        ] + srcs,
        main = "//tools/python/pytest:pytest_wrapper.py",
        args = [
            "--capture=no",
        ] + args + ["$(location :%s)" % x for x in srcs],
        python_version = "PY3",
        srcs_version = "PY3",
        deps = deps + [
            requirement("pytest"),
        ],
        **kwargs
    )
