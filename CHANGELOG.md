## [2.0.2](https://github.com/benadamstyles/bs-fluture/compare/v2.0.1...v2.0.2) (2021-01-05)


### Bug Fixes

* **deps:** bump ini from 1.3.5 to 1.3.8 ([d4c358f](https://github.com/benadamstyles/bs-fluture/commit/d4c358f0a874bd886c6458a6ac83d3277ecbd14e))

## [2.0.1](https://github.com/benadamstyles/bs-fluture/compare/v2.0.0...v2.0.1) (2020-12-29)


### Bug Fixes

* **deps:** bump node-notifier from 8.0.0 to 8.0.1 ([96ae887](https://github.com/benadamstyles/bs-fluture/commit/96ae8877bccb65d1d68bc8f4fea9050b6e0f9ad8))

# [2.0.0](https://github.com/benadamstyles/bs-fluture/compare/v1.0.0...v2.0.0) (2020-10-03)


### Bug Fixes

* **bsconfig:** remove namespacing ([432ed55](https://github.com/benadamstyles/bs-fluture/commit/432ed55b2048b172fde6ab407d0cfbe862fa7aad))


### Code Refactoring

* **core:** change the module's namespace from BsFluture to Fluture ([ad2cd8a](https://github.com/benadamstyles/bs-fluture/commit/ad2cd8a270440092e6fa17cfed088b8578b4f873))


### BREAKING CHANGES

* **core:** You will need to update your code to reference the `Fluture` module rather than `BsFluture`.

# 1.0.0 (2020-09-27)


### Code Refactoring

* **core:** upgrade bindings to support Fluture v12+ ([fb41957](https://github.com/benadamstyles/bs-fluture/commit/fb41957))


### BREAKING CHANGES

* **core:** The bindings have been updated to support Fluture v12+, which introduced major breaking changes to the Fluture API as detailed [here](https://gist.github.com/Avaq/ee2c6c819db4c37258e9a226e6380a38#functions-use-simple-currying).
