# DDS-Pipe Versions

This file includes the released versions of **DDS-Pipe** project along with their contributions to the project.
The *Forthcoming* section includes those features added in `main` branch that are not yet in a stable release.

## Forthcoming

* Store schemas in OMG IDL and ROS 2 msg format.

## Version 0.2.0

* Add missing DLLs.
* Add TSAN fixes.
* Restore default DomainParticipantQoS after creating and enabling ``DynTypesParticipant``.
* Display all information in DDS Topic log traces.
* Wait for all messages to be acknowledged in reliable CommonWriters destruction.
* Methods have been added on **YamlReader** class.
* Add debugging listener callbacks.
* Add customizable transport:
    * Ignore Participant Flags
    * Interface Whitelisting
    * Custom Transport Descriptors
* New ``CommonParticipant`` class for a Fast DDS Participant wrapper.
* New ``XmlParticipant`` class for a Fast DDS Participant in which the participant is created following a profile name loaded by XML configuration.
* New ``CommonReader`` class for a Fast DDS Data Reader wrapper.
* New ``SimpleReader`` class for a Fast DDS Data Reader that implements abstract ``CommonReader``.
* New ``SpecificQoSReader`` class for a Fast DDS Data Reader with specific QoS policies.
* New ``CommonWriter`` class for a Fast DDS Data Writer wrapper.
* New ``SimpleWriter`` class for a Fast DDS Data Writer that implements abstract ``CommonWriter``.
* New ``QoSSpecificWriter`` class for a Fast DDS Data Writer with specific QoS policies.
* New ``MultiWriter`` class for a Fast DDS Data Writer collection that contains multiple ``QoSSpecificWriter``.
* Dependencies update:
    |  | Repository | Old Version | New Version |
    |---|---|---|---|
    | Foonathan Memory Vendor | [eProsima/foonathan_memory_vendor](https://github.com/eProsima/foonathan_memory_vendor) | [v1.3.0](https://github.com/eProsima/foonathan_memory_vendor/releases/tag/v1.3.0) | [v1.3.1](https://github.com/eProsima/foonathan_memory_vendor/releases/tag/v1.3.1) |
    | Fast CDR | [eProsima/Fast-CDR](https://github.com/eProsima/Fast-CDR) | [v1.0.27](https://github.com/eProsima/Fast-CDR/releases/tag/v1.0.27) | [v1.1.0](https://github.com/eProsima/Fast-CDR/releases/tag/v1.1.0) |
    | Fast DDS | [eProsima/Fast-DDS](https://github.com/eProsima/Fast-DDS) | [v2.10.1](https://github.com/eProsima/Fast-DDS/releases/tag/v2.10.1) | [v2.11.0](https://github.com/eProsima/Fast-DDS/releases/tag/v2.11.0) |
    | Dev Utils | [eProsima/dev-utils](https://github.com/eProsima/dev-utils) | [v0.1.0](https://github.com/eProsima/dev-utils/releases/tag/v0.1.0) | [v0.2.0](https://github.com/eProsima/dev-utils/releases/tag/v0.2.0) |


## Version 0.1.0

First project release inheriting from DDS-Router `core` and `yaml`.
