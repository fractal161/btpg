import { defineConfig } from 'vite';
import { viteStaticCopy } from 'vite-plugin-static-copy';

// https://github.com/mycelial/mycelial-js/issues/25#issuecomment-1533305723
export default defineConfig({
  assetsInclude: ['**/*.onnx'],
  base: '/btpg',
  build: {
    rollupOptions: {
      output: {
        manualChunks: {
          onnx: ['onnxruntime-web'],
        },
      },
    },
  },
  plugins: [
    viteStaticCopy({
      targets: [
        {
          src: 'node_modules/onnxruntime-web/dist/*.wasm',
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
