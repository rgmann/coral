[![Build Status](https://travis-ci.org/rgmann/coral.svg?branch=master)](https://travis-ci.org/rgmann/coral)

## Welcome to CORAL

Coral is a small collection of C++ libraries. The libraries include:

 - coral::core

   A collection of basic containers, utilities, and wrappers.


 - coral::netapp

   A packet-routing framework for building networked applications. This framework allows you to decouple application logic from networking logic.


 - coral::rpc

   A Remote Procedure Call (RPC) framework built on coral::netapp and Google Protocol Buffers<sup>1</sup>.  A *protoc* plugin is provided to generate services directly from your *.protoc* service definitions.


## Supported Platforms

   - OSX
   - Linux (tested on Ubuntu)


## Dependencies

Coral depends on:

1. [CMake](https://cmake.org/) (>= 2.8)

    On OSX with Macports:

        port install cmake

    On Ubuntu:

        sudo apt-get install cmake


2. [Boost](http://www.boost.org/) (>= 1.47)

    On OSX with Macports:

        port install boost

    On Ubuntu:

        sudo apt-get install boost

    Or, on either platform, install manually, which is nearly as [easy](http://www.boost.org/doc/libs/1_59_0/more/getting_started/unix-variants.html).

    In all cases, multi-threading support should be enabled and tagged.


3. [GTest](https://code.google.com/p/googletest/)<sup>2</sup>

    1. [Download](https://code.google.com/p/googletest/downloads/list) and unzip GTest
    2. `cd gtest-x.x.x/`
    3. `cmake .`
    4. `make`
    5. Create the environment variable *GTEST_ROOT* to point to the root directory from step #2.


For RPC:

4. [Protocol Buffers](https://developers.google.com/protocol-buffers/)<sup>1</sup>

    See https://github.com/google/protobuf for installation instructions.


5.  Ruby and the Rake gem [OPTIONAL]

    A Rakefile is provided with the RPC service generator utility (in
    utils/rpc_generate). The Rakefile includes tasks to simplify coral::rpc
    service generation (type "rake help" for more information). This Rakefile
    is a convenience, but is by no means necessary. If you do want to use the
    Rakefile, simply duplicate it to the root of your project and adjust paths
    as necessary using the helper tasks (again, type "rake help" for details).


## Getting Started

Once the dependencies have been installed, you can build all Coral libraries,
utilities, examples, and tests by issuing the following commands from the Coral
root directory.

 1. Configure the project

        cmake .

 2.  Build the project

        make all

 3. Create an environment variable called CORALRPC_GEN_ROOT whose value is
       the full path to utils/rpc_generate.


## Examples

*  An example coral::rpc project can be found in examples/rpc/search_service.



 1. Copyright 2008, Google Inc.
 2. Copyright 2007, Google Inc.
