{ lib
, llvmPackages_23
, cmake
, ncurses }:

llvmPackages_23.stdenv.mkDerivation rec {
  pname = "gameoflife";
  version = "4.0.1";

  src = ./.;

  nativeBuildInputs = [ cmake ];
  buildInputs = [ ncurses ];

  cmakeFlags = [
    "-DENABLE_TESTING=OFF"
    "-DENABLE_INSTALL=ON"
  ];

  meta = with lib; {
    homepage = "https://github.com/sxlphuric/gameoflife";
    description = ''
      Simple C++ implementation of Conway's Game Of Life with ncurses";
    '';
    licencse = licenses.mit;
    platforms = with platforms; linux ++ darwin;
    maintainers = [ maintainers.breakds ];
  };
}
