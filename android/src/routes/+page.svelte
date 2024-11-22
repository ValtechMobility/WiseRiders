<script lang="ts">
  import { invoke } from "@tauri-apps/api/core";

  let ip = $state("192.168.125.250");
  let show_ip = $state(true);
  let log = $state("");
  let crashed = $state(false);
  let audio;

  function playAudio() {
    audio.play();
  }
  async function flash() {
    const response = await invoke("fetch_url", {
      url: `http://${ip}/`,
      method: "POST",
    });
  }

  async function start() {
    show_ip = false;

    setInterval(async () => {
      let response = await invoke("fetch_url", {
        url: `http://${ip}/`,
        method: "GET",
      });

      if (response === "GET method received. There was an accident.") {
        crashed = true;
      }
    }, 1000);
  }

  setTimeout(playAudio, 5000);

  $effect(() => {
    if (crashed) {
      playAudio();
    }
  });
</script>

<main class="container" class:crashed>
  {#if show_ip}
    <input type="text" bind:value={ip} />
    <button onclick={start}>Connect</button>
    <button onclick={playAudio}>Init audio</button>
  {:else}
    <button onclick={flash}
      ><img src="/honk.svg" alt="Honk  and flash" width="100" /></button
    >

    {#if crashed}
      <p>Driver crashed</p>
    {/if}
  {/if}
  <audio bind:this={audio} src="/crash.mp3" controls={show_ip}></audio>
</main>

<style>
  p {
    color: #fff;
    font-size: 30px;
    text-align: center;
    margin-top: 50px;
  }

  .container {
    display: flex;
    flex-direction: column;
    height: 100vh;
    justify-content: center;
    align-items: center;
  }

  .container.crashed {
    background: rgb(105, 13, 13);
    transition: 1s;
  }

  button {
    background: #6a6;
    width: 200px;
    height: 200px;
    border: 0;
    border-radius: 10px;
    box-shadow: 30px 30px 40px rgba(0, 0, 0, 0.3);
    color: #fff;
    font-size: 20px;
  }

  button:active {
    box-shadow: 10px 10px 40px rgba(0, 0, 0, 0.3);
    transform: translate3d(5px, 5px, 0);
    transition: all 0.1s;
  }
</style>
