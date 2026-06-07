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

unsigned long last_pub = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Use USB serial for micro-ROS transport
  set_microros_serial_transports(Serial);

  allocator = rcl_get_default_allocator();

  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "esp32_node", "", &support);

  rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "esp32_test"
  );
}

void loop() {
  if (millis() - last_pub > 500) {
    msg.data++;
    rcl_publish(&publisher, &msg, NULL);
    last_pub = millis();
  }

  delay(10);
}
