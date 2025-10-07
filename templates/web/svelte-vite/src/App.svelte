<script>
  // Simple component state tracked by Svelte's reactivity.
  let count = 0;
</script>
<main style="font-family: system-ui, sans-serif; margin: 2rem;">
  <h1 style="color:#2a5">Umicom — Svelte + Vite</h1>
  <p>Minimal Svelte starter configured for Vite.</p>
  <button on:click={() => count++}>Clicked {count} times</button>
</main>
