
const client = mqtt.connect("wss://broker.hivemq.com:8884/mqtt");

client.on("connect", function () {
  console.log("Connected to MQTT broker");
  client.subscribe("iot/traffic/WE/state");
  client.subscribe("iot/traffic/EW/state");
  client.subscribe("iot/traffic/WE/queue");
  client.subscribe("iot/traffic/EW/queue");
});

client.on("message", function (topic, message) {
  const msg = message.toString();
  if (topic === "iot/traffic/WE/state") {
    const light = document.getElementById("lightWE");
    light.className = "light " + msg.toLowerCase();
    light.textContent = msg;
  }
  if (topic === "iot/traffic/EW/state") {
    const light = document.getElementById("lightEW");
    light.className = "light " + msg.toLowerCase();
    light.textContent = msg;
  }
  if (topic === "iot/traffic/WE/queue") {
    document.getElementById("queueWE").textContent = msg;
  }
  if (topic === "iot/traffic/EW/queue") {
    document.getElementById("queueEW").textContent = msg;
  }
});
