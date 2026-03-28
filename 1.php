<img id="screen" alt="Image" loading="lazy" />
<script>
//this is a few settings
document.addEventListener('contextmenu', e => e.preventDefault());

document.querySelectorAll('img').forEach(img => {
  img.addEventListener('dragstart', e => e.preventDefault());
});


document.addEventListener('keydown', e => {
	// Block F12 (DevTools) this is so i can turn off and on the mic and camera for a later update
	if (e.key === 'F11'){
		e.preventDefault();
	}
});
</script>
<script>
//this is the video code
const ws_img = new WebSocket("ws://192.168.1.109:8084"); // change to your server address
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
//this is the audio code
let audioContext;
let AudioWS;//AudioWS = the WebSocket for Audio
let nextPlayTime = 0;
const audiobutton = document.getElementById("audio");

const SAMPLE_RATE = 44100; // ⚠️ MUST match your C++ quality value

function startAudio() {
	audiobutton.style.display = "none";
	
    audioContext = new (window.AudioContext || window.webkitAudioContext)({
        sampleRate: SAMPLE_RATE
    });

    AudioWS = new WebSocket("ws://192.168.1.109:8083");
    AudioWS.binaryType = "arraybuffer";

    AudioWS.onopen = () => {
        console.log("WebSocket connected");
        nextPlayTime = audioContext.currentTime;
    };

    AudioWS.onmessage = (event) => {

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
	//also not needed
    AudioWS.onerror = (err) => {
        console.error("WebSocket error:", err);
    };
}
</script>
<script>
//this is the keyboard code
//KeyWS = Keyboard WebSocket
const KeyWS = new WebSocket('ws://192.168.1.109:8082'); // replace with your server's address

KeyWS.binaryType = 'arraybuffer'; // tell it to receive binary, not text

KeyWS.addEventListener('open', () => {
    console.log('Connected!');
});

document.addEventListener('keydown', (Key) => {
	console.log(Key.keyCode + ' pressed');
	const buffer = new Int32Array([1 , Key.keyCode]).buffer; // 1 = pressed
	KeyWS.send(buffer);
});

document.addEventListener('keyup', (Key) => {
	console.log(Key.keyCode + ' released');
	const buffer = new Int32Array([0 , Key.keyCode]).buffer; // 1 = pressed
	KeyWS.send(buffer);
});

/*this is not needed
ws.addEventListener('message', (event) => { the c++ websocket can send back a msg
    // If C++ sends something back
    const arr = new Int32Array(event.data);
    console.log('Received:', arr);
});

ws.addEventListener('close', () => {//it shouldn't close but if it did the user needs to refrish [add later]
    console.log('Disconnected');
});

ws.addEventListener('error', (err) => {
    console.error('Error:', err);
});
*/
</script>
<script>
//this is the mouse code
let ScreenHeight = 0;
let ScreenWidth = 0;
//const S = streamElement.getBoundingClientRect();

const MouseWS = new WebSocket('ws://192.168.1.109:8081');

MouseWS.binaryType = 'arraybuffer';

MouseWS.addEventListener('open', () => {
    console.log('Connected!');
});
/*
document.addEventListener('mousemove', function(event) {
	var x = event.clientX;
	var y = event.clientY;
	console.log('Mouse position: X=' + x + ', Y=' + y);
	
	const buffer = new Int32Array([event.clientX , event.clientY]).buffer; // 1 = pressed
	MouseWS.send(buffer);
});
*/

document.addEventListener('mousemove', function(MousePos) {
	//MousePlaceX = Math.trunc(MousePos.clientX * (ScreenWidth  / window.innerWidth));
	//MousePlaceY = Math.trunc(MousePos.clientY * (ScreenHeight / window.innerHeight));

	MousePlaceX = Math.trunc(MousePos.clientX * (ScreenWidth  / img.width));
	MousePlaceY = Math.trunc(MousePos.clientY * (ScreenHeight / img.height));
	
	const buffer = new Int32Array([MousePlaceX , MousePlaceY , 1]).buffer; // 1 = pressed
	MouseWS.send(buffer);
	console.log('Mouse position: X=' + MousePos.clientX + ', Y=' + MousePos.clientY);

});

document.addEventListener('mousedown', function(Mouse) {
	const buffer = new Int32Array([Mouse.button , 1 , 0]).buffer;
	MouseWS.send(buffer);
	console.log(Mouse.button);
});

document.addEventListener('mouseup', function(Mouse) {
	const buffer = new Int32Array([Mouse.button , 0 , 0]).buffer;
	MouseWS.send(buffer);
});

MouseWS.onmessage = (Size) => {
    const view = new DataView(Size.data); // No await, no .arrayBuffer()
    ScreenWidth  = view.getInt32(0, true);
    ScreenHeight = view.getInt32(4, true);
    console.log(ScreenWidth, ScreenHeight);
};
//const availWidth = window.screen.availWidth;
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
*{
  -webkit-user-select: none;
  user-select: none;
}
</style>
