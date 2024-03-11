'''
small script to check that the onnx export actually worked
'''

import onnx
import onnxruntime
import torch

from models import BetaTetrisV1
# output is rotation, y, x

# verify the model's health
onnx_model = onnx.load("agents/betatetris-v1.onnx")
onnx.checker.check_model(onnx_model)

torch_model = BetaTetrisV1(192, 8)
torch_model.load_state_dict(torch.load('agents/betatetris-v1.pth', map_location=torch.device('cpu')))
torch_model.eval()

torch_input = torch.randn(1, 14, 20, 10, device=torch.device('cpu'))
torch_outputs = torch_model(torch_input)

# onnx_input = onnx_model.adapt_torch_inputs_to_onnx(torch_input)
# print(f"Input length: {len(onnx_input)}")
# print(f"Sample input: {onnx_input}")

ort_session = onnxruntime.InferenceSession("agents/betatetris-v1.onnx", providers=['CPUExecutionProvider'])

def to_numpy(tensor):
    return tensor.detach().cpu().numpy() if tensor.requires_grad else tensor.cpu().numpy()

ort_name = ort_session.get_inputs()[0].name
onnxruntime_input = { ort_name : to_numpy(torch_input) }
onnxruntime_outputs = ort_session.run(None, onnxruntime_input)

assert len(torch_outputs) == len(onnxruntime_outputs)
for torch_output, onnxruntime_output in zip(torch_outputs, onnxruntime_outputs):
    torch.testing.assert_close(torch_output, torch.tensor(onnxruntime_output))

# TODO: add V2
