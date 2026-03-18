<img id="screen" alt="Image" loading="lazy" />
<script>
const ws_img = new WebSocket("ws://putyourip\url:8084"); // change to your server address
const img = document.getElementById("screen");

ws_img.binaryType = "arraybuffer"; // ensure we receive raw bytes

ws_img.onopen = () => {
    console.log("WebSocket connected");
};

ws_img.onmessage = async (event) => {
    try {
        // Convert ArrayBuffer to a Blob
        const arrayBuffer = event.data;
        const blob = new Blob([arrayBuffer], { type: "image/jpeg" }); // or "image/png" if PNG
        const url = URL.createObjectURL(blob);

        // Show the image
        img.src = url;

        // Revoke the old URL to avoid memory leaks
        img.onload = () => URL.revokeObjectURL(url);
    } catch (e) {
        console.error("Error processing image:", e);
    }
};

ws_img.onclose = () => console.log("WebSocket closed");
ws_img.onerror = (err) => console.error("WebSocket error:", err);
</script>
<button onclick="startAudio()" id="audio" class="audiobutton">Start Audio</button>


<script>
let audioContext;
let ws;
let nextPlayTime = 0;
const audiobutton = document.getElementById("audio");

const SAMPLE_RATE = 44100; // ⚠️ MUST match your C++ quality value

function startAudio() {
	audiobutton.style.display = "none";
	
    audioContext = new (window.AudioContext || window.webkitAudioContext)({
        sampleRate: SAMPLE_RATE
    });

    ws = new WebSocket("ws://putyourip\url:8083");
    ws.binaryType = "arraybuffer";

    ws.onopen = () => {
        console.log("WebSocket connected");
        nextPlayTime = audioContext.currentTime;
    };

    ws.onmessage = (event) => {

        // DIRECT Float32 — no conversion
        const floatSamples = new Float32Array(event.data);

        const buffer = audioContext.createBuffer(
            1, // mono
            floatSamples.length,
            SAMPLE_RATE
        );

        buffer.copyToChannel(floatSamples, 0);

        const source = audioContext.createBufferSource();
        source.buffer = buffer;
        source.connect(audioContext.destination);

        // schedule to avoid gaps
        if (nextPlayTime < audioContext.currentTime)
            nextPlayTime = audioContext.currentTime;

        source.start(nextPlayTime);
        nextPlayTime += buffer.duration;
    };

    ws.onerror = (err) => {
        console.error("WebSocket error:", err);
    };
}
</script>
<style>
img{
	width: 100vw;
    height: 100vh;
}
body{
	margin:0px;
}
.audiobutton{
	width:100vw;
	height:100vh;
}
</style>
