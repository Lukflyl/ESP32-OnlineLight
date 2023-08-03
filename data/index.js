const socket = new WebSocket("ws://192.168.0.101/chat");
let isConnected = false;

socket.onopen = (e) => {
  document.getElementById("info").classList.remove("info-offline");
  document.getElementById("info").classList.add("info-online");
  document.getElementById("info").innerText = "Connected to the server :)";
  isConnected = true;
  sendValues();
};

socket.onclose = () => {
  document.getElementById("info").classList.remove("info-online");
  document.getElementById("info").classList.add("info-offline");
  document.getElementById("info").innerText = "Not connected to the server :(";
  isConnected = false;
};

const sendValues = () => {
  const redSlider = document.getElementById("redSlider");
  const greenSlider = document.getElementById("greenSlider");
  const blueSlider = document.getElementById("blueSlider");

  if (!isConnected) {
    return;
  }

  const data = {
    red: parseInt(redSlider.value),
    green: parseInt(greenSlider.value),
    blue: parseInt(blueSlider.value),
  };

  socket.send(JSON.stringify(data));
};
