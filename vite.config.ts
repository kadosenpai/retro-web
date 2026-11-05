import { defineConfig } from 'vite';
import { svelte } from '@sveltejs/vite-plugin-svelte';

// Header bắt buộc để bật cross-origin isolation (SharedArrayBuffer cho audio ring buffer).
// Khi deploy lên Cloudflare Pages, các header này nằm ở public/_headers.
const isolationHeaders = {
  'Cross-Origin-Opener-Policy': 'same-origin',
  'Cross-Origin-Embedder-Policy': 'require-corp',
  'Cross-Origin-Resource-Policy': 'cross-origin',
};

export default defineConfig({
  plugins: [svelte()],
  server: {
    host: true,
    port: 5173,
    headers: isolationHeaders,
  },
  preview: {
    host: true,
    port: 4173,
    headers: isolationHeaders,
  },
  worker: {
    format: 'es',
  },
  build: {
    target: 'es2022',
    sourcemap: true,
  },
});
