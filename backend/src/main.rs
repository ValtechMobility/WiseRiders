/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

use log::info;
use std::io::{Read, Write};
use std::sync::Arc;
use up_rust::{UListener, UMessage, UMessageBuilder, UPayloadFormat, UStatus, UTransport, UUri, UUID};
use up_client_mqtt5_rust::{MqttConfig, UPClientMqtt, UPClientMqttType};
use async_trait::async_trait;
use std::net::TcpListener;
use once_cell::sync::Lazy;
use std::sync::atomic::{AtomicBool, Ordering};

const MQTT_PORT: u16 = 1883;
const MQTT_HOSTNAME: &str = "test.mosquitto.org";
const MQTT_USERNAME: &str = "";

const PUB_TOPIC_AUTHORITY: &str = "Cloud";
const PUB_TOPIC_UE_ID: u32 = 0x3039;
const PUB_TOPIC_UE_VERSION_MAJOR: u8 = 1;
const PUB_TOPIC_RESOURCE_ID: u16 = 0x8001;

const SUB_TOPIC_AUTHORITY: &str = "MXChip";
const SUB_TOPIC_UE_ID: u32 = 0x5BA0;
const SUB_TOPIC_UE_VERSION_MAJOR: u8 = 1;
const SUB_TOPIC_RESOURCE_ID: u16 = 0x8001;

static ACCIDENT_STATUS: Lazy<AtomicBool> = Lazy::new(|| AtomicBool::new(false));

fn set_flag() {
    ACCIDENT_STATUS.store(true, Ordering::Relaxed);
}

fn check_flag() -> bool {
    ACCIDENT_STATUS.load(Ordering::Relaxed)
}

struct PublishReceiver;

#[async_trait]
impl UListener for PublishReceiver {
    async fn on_receive(&self, msg: UMessage) {
        info!("PublishReceiver: Received a message: {msg:?}");

        let Some(payload_bytes) = msg.payload else {
            panic!("No bytes available");
        };
        println!("Bytes: {:?}",payload_bytes);

        set_flag();
    }
}


async fn build_mqtt() -> Result<Arc<dyn UTransport>, UStatus> {
    env_logger::init();

    info!("Started Backend.");

    let ssl_options = None;

    let mqtt_config = MqttConfig {
        mqtt_protocol: up_client_mqtt5_rust::MqttProtocol::Mqtt,
        mqtt_port: MQTT_PORT,
        mqtt_hostname: MQTT_HOSTNAME.to_string(),
        max_buffered_messages: 100,
        max_subscriptions: 100,
        session_expiry_interval: 3600,
        ssl_options,
        username: MQTT_USERNAME.to_string(),
    };

    let publisher: Arc<dyn UTransport> = Arc::new(
        UPClientMqtt::new(
            mqtt_config,
            UUID::build(),
            PUB_TOPIC_AUTHORITY.to_string(),
            UPClientMqttType::Device,
        )
        .await
        .expect("Could not create mqtt transport."),
    );

    Ok(publisher)
}


#[tokio::main]
async fn main() {

    // This is the URI that we want to receive from
    let source_filter = UUri::try_from_parts(
        SUB_TOPIC_AUTHORITY,
        SUB_TOPIC_UE_ID,
        SUB_TOPIC_UE_VERSION_MAJOR,
        SUB_TOPIC_RESOURCE_ID,
    )
    .unwrap();

    let client = build_mqtt().await.unwrap();

    let publish_receiver: Arc<dyn UListener> = Arc::new(PublishReceiver);

    let listener_client = client.clone();
    listener_client.register_listener(&source_filter, None, publish_receiver.clone())
        .await.unwrap();
    
    let listener = TcpListener::bind(format!("0.0.0.0:{}", 80)).unwrap();

    loop {
        let send_client = client.clone();
        let (mut socket, _) = listener.accept().unwrap();
        tokio::spawn(async move {
            let mut buffer = [0; 1024];

            // Read data from the socket
            let n = socket.read(&mut buffer).unwrap();
            if n == 0 {
                return;
            }

            // Convert the buffer to a string for parsing
            let request = String::from_utf8_lossy(&buffer[..n]);

            // Match on the HTTP method
            if request.starts_with("GET") {
                println!("Received a GET request");
                let response = if check_flag() {
                    "HTTP/1.1 200 OK\r\n\r\nGET method received. There was an accident."
                } else {
                    "HTTP/1.1 200 OK\r\n\r\nGET method received. No accident"
                };
                socket.write_all(response.as_bytes()).unwrap();
            } else if request.starts_with("POST") {
                println!("Received a POST request");
                let response = "HTTP/1.1 200 OK\r\n\r\nPOST method received!";
                socket.write_all(response.as_bytes()).unwrap();

                let source = UUri::try_from_parts(
                    PUB_TOPIC_AUTHORITY,
                    PUB_TOPIC_UE_ID,
                    PUB_TOPIC_UE_VERSION_MAJOR,
                    PUB_TOPIC_RESOURCE_ID,
                )
                .unwrap();

                let message = UMessageBuilder::publish(source)
                    .build_with_payload(
                        vec![1],
                        UPayloadFormat::UPAYLOAD_FORMAT_TEXT,
                    )
                    .expect("Failed to build message");

                send_client.send(message).await.unwrap();
                info!("sent a message to chip!")

            } else {
                println!("Unknown request method");
                let response = "HTTP/1.1 400 Bad Request\r\n\r\nUnsupported method!";
                socket.write_all(response.as_bytes()).unwrap();
            }
        });
    };
}

#[cfg(test)]
mod test {
    use super::*;

    static ACCIDENT_STATUS: Lazy<AtomicBool> = Lazy::new(|| AtomicBool::new(false));

    fn set_flag() {
        ACCIDENT_STATUS.store(true, Ordering::Relaxed);
    }

    fn check_flag() -> bool {
        ACCIDENT_STATUS.load(Ordering::Relaxed)
    }

    struct TestListener;

    #[async_trait]
    impl UListener for TestListener {
        async fn on_receive(&self, _msg: UMessage) {
            set_flag();
        }
    }

    #[tokio::test]
    async fn test_mqtt_init() {
        let client = build_mqtt().await.unwrap();

        let test_receiver: Arc<dyn UListener> = Arc::new(PublishReceiver);

        let test_filter = UUri::try_from("test").unwrap();

        client.register_listener(&test_filter, None, test_receiver.clone())
        .await.unwrap();

        let message = UMessageBuilder::publish(test_filter)
                    .build_with_payload(
                        vec![1],
                        UPayloadFormat::UPAYLOAD_FORMAT_TEXT,
                    )
                    .expect("Failed to build message");

        client.send(message).await.unwrap();

        assert_eq!(check_flag(), true)
    }
}