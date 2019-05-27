# UE4 DirectX 12 Mip Generation

## Disclaimer
Those engine changes are considered experimental and may be modified in future releases. Use it at your own risk.

## Overview

New directX 12 api has no more special function GenerateMips(). Accordingly to MS documentation developers need to implement this mechanic by themselves. This engine update adds compute shader that solves that problem. Also, this update fixes problem with mip auto generation in RenderTarget assets.

## Getting Started

Just download UnrealEngine-4.22.0 folder from repository and move/replace its contents to your engine folder. You will also have to recompile your engine and project afterwards.

### Prerequisites

You have to run you project on UE-4.22.0 compiled from sources. All prerequisites are the same as for running the usual engine. Merging and using this update with other versions of the engine was not tested, however may be possible. Please let us know about your experience in the comments.

### Using mip generation functionality


### Roadmap

- add support for other texture types (Cube textures, 3d textures, texture arrays)

## Contributing

Please read [CONTRIBUTING.md](Documentation/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/fracturedbyte/UE4-MaterialBlending/tags). 

## Authors

* **Gleb Bulgakov** - *Initial coding* - [FracturedByte](https://github.com/BulgakovGleb)

See also the list of [contributors](https://github.com/fracturedbyte/UE4-MultiViewRendering/contributors) who participated in this project.

## Acknowledgments

* Many thanks for [PurpleBooth](https://gist.github.com/PurpleBooth/) for making templates of [CONTRIBUTION.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) and [README.md](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2) that we've used for this repo
* Inspired by needs of UE4 developers
