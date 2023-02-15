workspace(name="dragonbook")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# C++ CONFIG
http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip"],
  strip_prefix = "googletest-b796f7d44681514f58a683a3a71ff17c94edb0c1",
)

# GO CONFIG

http_archive(
    name = "io_bazel_rules_go",
    sha256 = "f2dcd210c7095febe54b804bb1cd3a58fe8435a909db2ec04e31542631cf715c",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_go/releases/download/v0.31.0/rules_go-v0.31.0.zip",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.31.0/rules_go-v0.31.0.zip",
    ],
)

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")

go_rules_dependencies()
go_register_toolchains(version = "1.18.1")

# Rust CONFIG

http_archive(
    name = "rules_rust",
    sha256 = "696b01deea96a5e549f1b5ae18589e1bbd5a1d71a36a243b5cf76a9433487cf2",
    urls = [
        "https://github.com/bazelbuild/rules_rust/releases/download/0.11.0/rules_rust-v0.11.0.tar.gz"
    ],
)

load("@rules_rust//rust:repositories.bzl",
     "rules_rust_dependencies",
     "rust_register_toolchains",
     "rust_analyzer_toolchain_repository",
)

rules_rust_dependencies()
rust_register_toolchains(edition="2021", version = "1.65.0")

load("@rules_rust//crate_universe:repositories.bzl", "crate_universe_dependencies")
crate_universe_dependencies()

load("@rules_rust//crate_universe:defs.bzl", "crates_repository", "crate", "render_config")

crates_repository(
    name = "crate_index",
    # Update lock file running: CARGO_BAZEL_REPIN=true bazel build //...
    cargo_lockfile = "//rust/config:cargo.bazel.lock",
    packages = {
        "stringreader": crate.spec(
            version = "0.1.1",
        ),
        "once_cell": crate.spec(
            version = "1.17.1",
        )
        # "memmap2": crate.spec(
        #     version = "0.5.8",
        # ),
        # "libc": crate.spec(
        #     version = "0.2.105",
        # ),
        # "memfile": crate.spec(
        #     version = "0.2.1",
        # ),
    },
    render_config = render_config(
        default_package_name = ""
    ),
)

load("@crate_index//:defs.bzl", "crate_repositories")
crate_repositories()

load("@rules_rust//tools/rust_analyzer:deps.bzl", "rust_analyzer_dependencies")
rust_analyzer_dependencies()

# To initialize run: bazel run @rules_rust//tools/rust_analyzer:gen_rust_project
register_toolchains(rust_analyzer_toolchain_repository(
    name = "rust_analyzer_toolchain",
    # This should match the currently registered toolchain.
    version = "1.65.0",
))

# Java CONFIG

RULES_JVM_EXTERNAL_TAG = "4.5"
RULES_JVM_EXTERNAL_SHA = "b17d7388feb9bfa7f2fa09031b32707df529f26c91ab9e5d909eb1676badd9a6"

http_archive(
    name = "rules_jvm_external",
    strip_prefix = "rules_jvm_external-%s" % RULES_JVM_EXTERNAL_TAG,
    sha256 = RULES_JVM_EXTERNAL_SHA,
    url = "https://github.com/bazelbuild/rules_jvm_external/archive/%s.zip" % RULES_JVM_EXTERNAL_TAG,
)

load("@rules_jvm_external//:repositories.bzl", "rules_jvm_external_deps")

rules_jvm_external_deps()

load("@rules_jvm_external//:setup.bzl", "rules_jvm_external_setup")

rules_jvm_external_setup()

load("@rules_jvm_external//:defs.bzl", "maven_install")
load("@rules_jvm_external//:specs.bzl", "maven")

maven_install(
    artifacts = [
        maven.artifact("junit", "junit", "4.13", testonly = True),
    ],
    repositories = [
        "https://maven.google.com",
        "https://repo1.maven.org/maven2",
    ],
)

