import { defineConfig } from 'vite';
import { viteStaticCopy } from 'vite-plugin-static-copy';

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
      ],
    }),
  ],
});
