(() => {
  const state = { zone: false, gps: true };
  const $ = (id) => document.getElementById(id);
  const log = $("eventLog");
  const start = Date.now();
  function timestamp() {
    const total = Math.floor((Date.now() - start) / 1000);
    const h = String(Math.floor(total / 3600)).padStart(2, "0");
    const m = String(Math.floor((total % 3600) / 60)).padStart(2, "0");
    const s = String(total % 60).padStart(2, "0");
    return `${h}:${m}:${s}`;
  }
  function addLog(message) {
    const li = document.createElement("li");
    const time = document.createElement("time");
    const span = document.createElement("span");
    time.textContent = timestamp();
    span.textContent = message;
    li.append(time, span);
    log.prepend(li);
    while (log.children.length > 8) log.removeChild(log.lastElementChild);
  }
  function render() {
    const pwm = state.gps ? (state.zone ? 65 : 110) : 0;
    $("modeValue").textContent = state.zone ? "LIMITED" : "NORMAL";
    $("modeValue").style.color = state.zone ? "var(--amber)" : "var(--mint)";
    $("modeNote").textContent = state.zone ? "Entry tag accepted (simulated)" : "No zone tag active";
    $("pwmValue").textContent = `${pwm} / 255`;
    $("gpsValue").textContent = state.gps ? "AVAILABLE" : "UNAVAILABLE";
    $("gpsValue").style.color = state.gps ? "var(--mint)" : "var(--red)";
    $("pwmPercent").textContent = `${Math.round(pwm / 255 * 100)}%`;
    $("pwmBar").style.width = `${pwm / 255 * 100}%`;
    $("pwmBar").style.background = state.zone ? "var(--amber)" : "var(--mint)";
  }
  $("entryBtn").addEventListener("click", () => {
    state.zone = true; addLog("Simulated RFID entry accepted; reduced model PWM selected."); render();
  });
  $("exitBtn").addEventListener("click", () => {
    state.zone = false; addLog("Simulated RFID exit accepted; normal model PWM selected."); render();
  });
  $("gpsBtn").addEventListener("click", () => {
    state.gps = !state.gps;
    addLog(state.gps ? "GPS status restored (simulated)." : "GPS unavailable (simulated); model command set to zero.");
    render();
  });
  $("clearBtn").addEventListener("click", () => { log.replaceChildren(); addLog("Log cleared."); });
  render();
})();
