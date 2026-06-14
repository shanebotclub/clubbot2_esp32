#define MICROROS_TRANSPORT_CUSTOM
#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>

rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

unsigned long last_time = 0;

void setup() {
  Serial.begin(115200);
  set_microros_transports();


  delay(2000);

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support);
  rclc_publisher_init_default(
      &publisher,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "micro_ros_arduino_node_publisher");

  msg.data = 0;
}

void loop() {
  if (millis() - last_time > 1000) {
    msg.data++;
    rcl_publish(&publisher, &msg, NULL);
    last_time = millis();
  }

  delay(10);
}

