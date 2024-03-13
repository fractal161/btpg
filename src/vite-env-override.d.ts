/// <reference types="vite/client" />

// static imports
declare module '*.onnx' {
    const src: string;
    export default src;
}
