import { defineConfig } from 'vite';
import { viteStaticCopy } from 'vite-plugin-static-copy';
import tailwindcss from '@tailwindcss/vite'

// https://github.com/mycelial/mycelial-js/issues/25#issuecomment-1533305723
export default defineConfig({
  assetsInclude: ['**/*.onnx'],
  base: '/btpg/',
  build: {
    rollupOptions: {
      output: {
        manualChunks: {
          onnx: ['onnxruntime-web'],
        },
      },
    },
  },
  esbuild: {
    supported: {
      'top-level-await': true // browsers can handle top-level-await features
    },
  },
  plugins: [
    tailwindcss(),
    viteStaticCopy({
      targets: [
        {
          src: 'node_modules/onnxruntime-web/dist/*.{wasm,mjs}',
          dest: '',
        },
        {
          src: 'wasm/*.wasm',
          dest: '',
        },
      ],
    }),
  ],
});
