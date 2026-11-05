<script lang="ts">
  // Màn kiểm tra môi trường của Giai đoạn 0 (sẽ được thay bằng UI thật ở Giai đoạn 7).
  type Check = { label: string; ok: boolean };

  const checks: Check[] = [
    { label: 'crossOriginIsolated', ok: self.crossOriginIsolated === true },
    { label: 'SharedArrayBuffer', ok: typeof SharedArrayBuffer !== 'undefined' },
    { label: 'WebAssembly', ok: typeof WebAssembly === 'object' },
    { label: 'Web Worker', ok: typeof Worker !== 'undefined' },
    { label: 'OffscreenCanvas', ok: typeof OffscreenCanvas !== 'undefined' },
    { label: 'AudioWorklet', ok: typeof AudioWorkletNode !== 'undefined' },
    { label: 'IndexedDB', ok: typeof indexedDB !== 'undefined' },
    { label: 'Gamepad API', ok: typeof navigator.getGamepads === 'function' },
    { label: 'Service Worker', ok: 'serviceWorker' in navigator },
  ];

  const allOk = checks.every((c) => c.ok);
</script>

<main>
  <h1>Retro Web Player</h1>
  <p class="sub">Giai đoạn 0: kiểm tra môi trường</p>

  <ul>
    {#each checks as c (c.label)}
      <li class:ok={c.ok} class:fail={!c.ok}>
        <span class="dot" aria-hidden="true"></span>
        <code>{c.label}</code>
        <strong>{c.ok ? 'OK' : 'KHÔNG'}</strong>
      </li>
    {/each}
  </ul>

  <p class="status" class:ok={allOk} class:fail={!allOk}>
    {allOk ? 'Sẵn sàng cho Giai đoạn 1.' : 'Có mục chưa đạt, xem danh sách ở trên.'}
  </p>
</main>

<style>
  main {
    max-width: 420px;
    margin: 0 auto;
    padding: 32px 20px;
  }
  h1 {
    margin: 0;
    font-size: 1.5rem;
  }
  .sub {
    margin: 4px 0 24px;
    opacity: 0.6;
  }
  ul {
    list-style: none;
    margin: 0;
    padding: 0;
    display: grid;
    gap: 8px;
  }
  li {
    display: flex;
    align-items: center;
    gap: 10px;
    padding: 10px 12px;
    border-radius: 8px;
    background: rgb(255 255 255 / 0.05);
  }
  li code {
    flex: 1;
  }
  .dot {
    width: 10px;
    height: 10px;
    border-radius: 50%;
    background: currentColor;
  }
  .ok {
    color: #4ade80;
  }
  .fail {
    color: #f87171;
  }
  .status {
    margin-top: 20px;
    font-weight: 600;
  }
</style>
