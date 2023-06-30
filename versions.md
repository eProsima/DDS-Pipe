# DDS-Pipe Versions

This file includes the released versions of **DDS-Pipe** project along with their contributions to the project.
The *Forthcoming* section includes those features added in `main` branch that are not yet in a stable release.

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

## Version 0.1.0

First project release inheriting from DDS-Router `core` and `yaml`.
