// This file is generated by rust-protobuf 3.6.0. Do not edit
// .proto file is parsed by protoc 28.2
// @generated

// https://github.com/rust-lang/rust-clippy/issues/702
#![allow(unknown_lints)]
#![allow(clippy::all)]

#![allow(unused_attributes)]
#![cfg_attr(rustfmt, rustfmt::skip)]

#![allow(dead_code)]
#![allow(missing_docs)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(non_upper_case_globals)]
#![allow(trivial_casts)]
#![allow(unused_results)]
#![allow(unused_mut)]

//! Generated file from `uprotocol/v1/ucode.proto`

/// Generated files are compatible only with the same version
/// of protobuf runtime.
const _PROTOBUF_VERSION_CHECK: () = ::protobuf::VERSION_3_6_0;

#[derive(Clone,Copy,PartialEq,Eq,Debug,Hash)]
// @@protoc_insertion_point(enum:uprotocol.v1.UCode)
pub enum UCode {
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.OK)
    OK = 0,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.CANCELLED)
    CANCELLED = 1,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.UNKNOWN)
    UNKNOWN = 2,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.INVALID_ARGUMENT)
    INVALID_ARGUMENT = 3,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.DEADLINE_EXCEEDED)
    DEADLINE_EXCEEDED = 4,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.NOT_FOUND)
    NOT_FOUND = 5,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.ALREADY_EXISTS)
    ALREADY_EXISTS = 6,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.PERMISSION_DENIED)
    PERMISSION_DENIED = 7,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.UNAUTHENTICATED)
    UNAUTHENTICATED = 16,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.RESOURCE_EXHAUSTED)
    RESOURCE_EXHAUSTED = 8,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.FAILED_PRECONDITION)
    FAILED_PRECONDITION = 9,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.ABORTED)
    ABORTED = 10,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.OUT_OF_RANGE)
    OUT_OF_RANGE = 11,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.UNIMPLEMENTED)
    UNIMPLEMENTED = 12,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.INTERNAL)
    INTERNAL = 13,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.UNAVAILABLE)
    UNAVAILABLE = 14,
    // @@protoc_insertion_point(enum_value:uprotocol.v1.UCode.DATA_LOSS)
    DATA_LOSS = 15,
}

impl ::protobuf::Enum for UCode {
    const NAME: &'static str = "UCode";

    fn value(&self) -> i32 {
        *self as i32
    }

    fn from_i32(value: i32) -> ::std::option::Option<UCode> {
        match value {
            0 => ::std::option::Option::Some(UCode::OK),
            1 => ::std::option::Option::Some(UCode::CANCELLED),
            2 => ::std::option::Option::Some(UCode::UNKNOWN),
            3 => ::std::option::Option::Some(UCode::INVALID_ARGUMENT),
            4 => ::std::option::Option::Some(UCode::DEADLINE_EXCEEDED),
            5 => ::std::option::Option::Some(UCode::NOT_FOUND),
            6 => ::std::option::Option::Some(UCode::ALREADY_EXISTS),
            7 => ::std::option::Option::Some(UCode::PERMISSION_DENIED),
            16 => ::std::option::Option::Some(UCode::UNAUTHENTICATED),
            8 => ::std::option::Option::Some(UCode::RESOURCE_EXHAUSTED),
            9 => ::std::option::Option::Some(UCode::FAILED_PRECONDITION),
            10 => ::std::option::Option::Some(UCode::ABORTED),
            11 => ::std::option::Option::Some(UCode::OUT_OF_RANGE),
            12 => ::std::option::Option::Some(UCode::UNIMPLEMENTED),
            13 => ::std::option::Option::Some(UCode::INTERNAL),
            14 => ::std::option::Option::Some(UCode::UNAVAILABLE),
            15 => ::std::option::Option::Some(UCode::DATA_LOSS),
            _ => ::std::option::Option::None
        }
    }

    fn from_str(str: &str) -> ::std::option::Option<UCode> {
        match str {
            "OK" => ::std::option::Option::Some(UCode::OK),
            "CANCELLED" => ::std::option::Option::Some(UCode::CANCELLED),
            "UNKNOWN" => ::std::option::Option::Some(UCode::UNKNOWN),
            "INVALID_ARGUMENT" => ::std::option::Option::Some(UCode::INVALID_ARGUMENT),
            "DEADLINE_EXCEEDED" => ::std::option::Option::Some(UCode::DEADLINE_EXCEEDED),
            "NOT_FOUND" => ::std::option::Option::Some(UCode::NOT_FOUND),
            "ALREADY_EXISTS" => ::std::option::Option::Some(UCode::ALREADY_EXISTS),
            "PERMISSION_DENIED" => ::std::option::Option::Some(UCode::PERMISSION_DENIED),
            "UNAUTHENTICATED" => ::std::option::Option::Some(UCode::UNAUTHENTICATED),
            "RESOURCE_EXHAUSTED" => ::std::option::Option::Some(UCode::RESOURCE_EXHAUSTED),
            "FAILED_PRECONDITION" => ::std::option::Option::Some(UCode::FAILED_PRECONDITION),
            "ABORTED" => ::std::option::Option::Some(UCode::ABORTED),
            "OUT_OF_RANGE" => ::std::option::Option::Some(UCode::OUT_OF_RANGE),
            "UNIMPLEMENTED" => ::std::option::Option::Some(UCode::UNIMPLEMENTED),
            "INTERNAL" => ::std::option::Option::Some(UCode::INTERNAL),
            "UNAVAILABLE" => ::std::option::Option::Some(UCode::UNAVAILABLE),
            "DATA_LOSS" => ::std::option::Option::Some(UCode::DATA_LOSS),
            _ => ::std::option::Option::None
        }
    }

    const VALUES: &'static [UCode] = &[
        UCode::OK,
        UCode::CANCELLED,
        UCode::UNKNOWN,
        UCode::INVALID_ARGUMENT,
        UCode::DEADLINE_EXCEEDED,
        UCode::NOT_FOUND,
        UCode::ALREADY_EXISTS,
        UCode::PERMISSION_DENIED,
        UCode::UNAUTHENTICATED,
        UCode::RESOURCE_EXHAUSTED,
        UCode::FAILED_PRECONDITION,
        UCode::ABORTED,
        UCode::OUT_OF_RANGE,
        UCode::UNIMPLEMENTED,
        UCode::INTERNAL,
        UCode::UNAVAILABLE,
        UCode::DATA_LOSS,
    ];
}

impl ::protobuf::EnumFull for UCode {
    fn enum_descriptor() -> ::protobuf::reflect::EnumDescriptor {
        static descriptor: ::protobuf::rt::Lazy<::protobuf::reflect::EnumDescriptor> = ::protobuf::rt::Lazy::new();
        descriptor.get(|| file_descriptor().enum_by_package_relative_name("UCode").unwrap()).clone()
    }

    fn descriptor(&self) -> ::protobuf::reflect::EnumValueDescriptor {
        let index = match self {
            UCode::OK => 0,
            UCode::CANCELLED => 1,
            UCode::UNKNOWN => 2,
            UCode::INVALID_ARGUMENT => 3,
            UCode::DEADLINE_EXCEEDED => 4,
            UCode::NOT_FOUND => 5,
            UCode::ALREADY_EXISTS => 6,
            UCode::PERMISSION_DENIED => 7,
            UCode::UNAUTHENTICATED => 8,
            UCode::RESOURCE_EXHAUSTED => 9,
            UCode::FAILED_PRECONDITION => 10,
            UCode::ABORTED => 11,
            UCode::OUT_OF_RANGE => 12,
            UCode::UNIMPLEMENTED => 13,
            UCode::INTERNAL => 14,
            UCode::UNAVAILABLE => 15,
            UCode::DATA_LOSS => 16,
        };
        Self::enum_descriptor().value_by_index(index)
    }
}

impl ::std::default::Default for UCode {
    fn default() -> Self {
        UCode::OK
    }
}

impl UCode {
    fn generated_enum_descriptor_data() -> ::protobuf::reflect::GeneratedEnumDescriptorData {
        ::protobuf::reflect::GeneratedEnumDescriptorData::new::<UCode>("UCode")
    }
}

static file_descriptor_proto_data: &'static [u8] = b"\
    \n\x18uprotocol/v1/ucode.proto\x12\x0cuprotocol.v1*\xb8\x02\n\x05UCode\
    \x12\x06\n\x02OK\x10\0\x12\r\n\tCANCELLED\x10\x01\x12\x0b\n\x07UNKNOWN\
    \x10\x02\x12\x14\n\x10INVALID_ARGUMENT\x10\x03\x12\x15\n\x11DEADLINE_EXC\
    EEDED\x10\x04\x12\r\n\tNOT_FOUND\x10\x05\x12\x12\n\x0eALREADY_EXISTS\x10\
    \x06\x12\x15\n\x11PERMISSION_DENIED\x10\x07\x12\x13\n\x0fUNAUTHENTICATED\
    \x10\x10\x12\x16\n\x12RESOURCE_EXHAUSTED\x10\x08\x12\x17\n\x13FAILED_PRE\
    CONDITION\x10\t\x12\x0b\n\x07ABORTED\x10\n\x12\x10\n\x0cOUT_OF_RANGE\x10\
    \x0b\x12\x11\n\rUNIMPLEMENTED\x10\x0c\x12\x0c\n\x08INTERNAL\x10\r\x12\
    \x0f\n\x0bUNAVAILABLE\x10\x0e\x12\r\n\tDATA_LOSS\x10\x0fB(\n\x18org.ecli\
    pse.uprotocol.v1B\nUCodeProtoP\x01b\x06proto3\
";

/// `FileDescriptorProto` object which was a source for this generated file
fn file_descriptor_proto() -> &'static ::protobuf::descriptor::FileDescriptorProto {
    static file_descriptor_proto_lazy: ::protobuf::rt::Lazy<::protobuf::descriptor::FileDescriptorProto> = ::protobuf::rt::Lazy::new();
    file_descriptor_proto_lazy.get(|| {
        ::protobuf::Message::parse_from_bytes(file_descriptor_proto_data).unwrap()
    })
}

/// `FileDescriptor` object which allows dynamic access to files
pub fn file_descriptor() -> &'static ::protobuf::reflect::FileDescriptor {
    static generated_file_descriptor_lazy: ::protobuf::rt::Lazy<::protobuf::reflect::GeneratedFileDescriptor> = ::protobuf::rt::Lazy::new();
    static file_descriptor: ::protobuf::rt::Lazy<::protobuf::reflect::FileDescriptor> = ::protobuf::rt::Lazy::new();
    file_descriptor.get(|| {
        let generated_file_descriptor = generated_file_descriptor_lazy.get(|| {
            let mut deps = ::std::vec::Vec::with_capacity(0);
            let mut messages = ::std::vec::Vec::with_capacity(0);
            let mut enums = ::std::vec::Vec::with_capacity(1);
            enums.push(UCode::generated_enum_descriptor_data());
            ::protobuf::reflect::GeneratedFileDescriptor::new_generated(
                file_descriptor_proto(),
                deps,
                messages,
                enums,
            )
        });
        ::protobuf::reflect::FileDescriptor::new_generated_2(generated_file_descriptor)
    })
}
