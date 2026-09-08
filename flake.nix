{
  description = "Game of life - C++ implementation";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = inputs: {
    packages =
      builtins.mapAttrs (system: pkgs: {
        gameoflife = {
          lib,
          stdenv,
        }:
          stdenv.mkDerivation {
            pname = "gameoflife";
            version = "4.0.1";

            src = let
              files = ./main.cpp;
            in
              lib.fileset.toSource {
                root = ./.;
                fileset = lib.fileset.trace ./main.cpp;
              };
          };

        default = inputs.self.packages.${system}.gameoflife;
      })
      inputs.nixpkgs.legacyPackages;
  };
}
