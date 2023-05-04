from conan import ConanFile

class Bruteforce(ConanFile):
    name = 'Bruteforce'
    version = '1.0'
    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'CMakeToolchain', 'CMakeDeps'
    requires = 'openssl/3.1.0', 'rapidjson/cci.20220822', 'wil/1.0.230202.1'