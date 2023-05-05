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
      buildGodot = inputs.godot-flake.lib.buildGodot;

      # godot editor for demo testing
      godot-editor = buildGodot.mkGodot {withTemplates = false;};

      # implementation
      gdosc = buildGdExt.buildExt { 
          extName = "gdosc";
          src = self;
          installPhase = ''
            mkdir -p $out/bin
            cp bin/libgdosc*.so $out/bin/libgdosc.so
            cp gdosc.gdextension $out/gdosc.gdextension
            '';
        };

      demo = gdosc.overrideAttrs (oldAttr : {
        nativeBuildInputs = oldAttr.nativeBuildInputs ++ [godot-editor];
        installPhase = oldAttr.installPhase +
        ''
          cp -r demo $out/demo
          mkdir -p $out/demo/godot/bin
          cp $out/bin/libgdosc.so $out/demo/godot/bin/libgdosc.so
        '';
      });
        
    in {
      #interface
      packages."${system}" = with pkgs; {
        inherit gdosc;
        default = demo;
      };
      # dev-shell
      devShells."${system}".default = buildGdExt.mkExtensionShell  gdosc;
    };
}