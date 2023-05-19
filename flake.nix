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
      libGDExt = inputs.godot-flake.lib.libGDExt;
      libGodot = inputs.godot-flake.lib.libGodot;

      # godot editor for demo testing
      godot-editor = libGodot.mkGodot {withTemplates = false;};

      # implementation
      gdosc = libGDExt.mkGDExt { 
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
          mkdir -p $out/demo/godot/bin/x11
          ln $out/bin/libgdosc.so $out/demo/godot/bin/x11/libgdosc.so
          cp gdosc.gdextension $out/demo/godot/bin
          mkdir -p $out/godot-engine/bin
          cp ${godot-editor}/bin/* $out/godot-engine/bin
        '';
      });
        
    in {
      #interface
      packages."${system}" = with pkgs; {
        inherit gdosc;
        default = demo;
      };
      # dev-shell
      devShells."${system}".default = (libGDExt.mkExtensionShell gdosc).overrideAttrs (oldAttr:
      {
         inputsFrom = [demo];
         shellHook = oldAttr.shellHook + ''
            ln -s ${godot-editor}/bin/*
            mkdir -p demo/godot/bin/x11
            ln -s  ../../../gdosc.gdextension demo/godot/bin/gdosc.gdextension 
         '';
      });
    };
}