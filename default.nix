{
  lib,
  llvmPackages_23,
  cmake,
  spdlog,
  abseil-cpp,
}: let
  files = ./main.cpp;
in
  lib.fileset.trace files
  llvmPackages_23.stdenv.mkDerivation {
    pname = "gameoflife";
    version = "4.0.1";

    src = lib.fileset.toSource {
      root = ./.;
      fileset = files;
    };
  }
