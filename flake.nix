{
  description = "Game of life - C++ implementation";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = inputs: let
  in {
    packages =
      builtins.mapAttrs (system: pkgs: {
        gameoflife = pkgs.callPackage ./default.nix {};

        default = inputs.self.packages.${system}.gameoflife;
      })
      inputs.nixpkgs.legacyPackages;
  };
}
