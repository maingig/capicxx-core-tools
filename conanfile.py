from conans import ConanFile, tools

# Must first use Maven to build binaries before using this file - see README

import os

class CapicxxcoretoolsConan(ConanFile):
    name = "capicxx-core-tools"
    version = "3.2.0.1"
    settings = "os", "compiler", "build_type", "arch"
    description = "Core tools built with maven"
    url = "None"
    license = "None"
    author = "None"
    topics = None

    def package(self):
        self.copy("*", "bin")

    def package_info(self):
        self.env_info.PATH.append(os.path.join(self.package_folder, "bin"))

