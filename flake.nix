# this flakes uses godot-flake to build gdosc
{
  description = "gdosc to enable osc in Godot";
  inputs = {

    # the godot Engine
    godot-flake = {
      url = "github:MadMcCrow/godot-flake";
    };
  };

  # func
  outputs = { self, nixpkgs, ... }@inputs:
    let
      # only linux supported, todo, support darwin
      system = "x86_64-linux";
      # use nixpkgs
      pkgs = import nixpkgs { inherit system; };
      lib = pkgs.lib;
      # get function from other flake
      buildGdExt = inputs.godot-flake.lib.buildGdExt;

      # implementation
      gdosc = buildGdExt.buildExt { 
          extName = "gdosc";
          src = self;
          nativeBuildInputs = [pkgs.breakpointHook];
        };
        
    in {

      #interface
      packages."${system}" = with pkgs; {

        default = gdosc;
       
      };
      # dev-shell
      #devShells."${system}".default = with pkgs; mkShell {
      #   nativeBuildInputs = [breakpointHook cntr] ++ gdosc.nativeBuildInputs;
      #   buildInputs = [] ++ gdosc.buildInputs;
      # };
    };
}