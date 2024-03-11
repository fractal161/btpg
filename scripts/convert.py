'''
BetaTetris is written in pytorch, which can't be run in the browser by
defeault. Instead, we convert the model to the ONNX format, where we can
leverage the ONNX Runtime to run the converted model in the browser.

Source: https://pytorch.org/tutorials/beginner/onnx/export_simple_model_to_onnx_tutorial.html
'''
import re

import numpy as np
import torch

from models import BetaTetrisV1, BetaTetrisV2
from models.v2 import obs_to_torch
# output is rotation, y, x

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

# V1 CODE
model = BetaTetrisV1(192, 8)
model.load_state_dict(torch.load('agents/betatetris-v1.pth', map_location=torch.device('cpu')))

# see https://github.com/pytorch/pytorch/issues/99662
model.eval()
dummy_input = torch.randn(1, 14, 20, 10, device=torch.device('cpu')) # random input for onnx
# torch.onnx.export(model, dummy_input, "test.onnx")
onnx_program = torch.onnx.dynamo_export(model, dummy_input)
onnx_program.save("agents/betatetris-v1.onnx")

# V2 CODE (TODO: make this work)
# https://github.com/adrien1018/betatetris-tablebase/blob/255027f608125a41463c79599b157c5cf2fecc26/python/fceux.py#L201
# with torch.no_grad():
#     state_dict = torch.load('agents/betatetris-v2-30hz-18f.pth', map_location=torch.device('cpu'))
#     channels = state_dict['main_start.0.main.0.weight'].shape[0]
#     start_blocks = len([0 for i in state_dict if re.fullmatch(r'main_start.*main\.0\.weight', i)])
#     end_blocks = len([0 for i in state_dict if re.fullmatch(r'main_end.*main\.0\.weight', i)])
#     model = BetaTetrisV2(start_blocks, end_blocks, channels).to(device)
#     model.load_state_dict(state_dict)
#     model.eval()
# 
#     # random input for onnx to use
#     dummy_input = obs_to_torch([
#         np.random.rand(6, 20, 10).astype('float32'),
#         np.random.rand(28).astype('float32'),
#         np.random.rand(14, 20, 10).astype('float32'),
#         np.random.rand(28).astype('float32'),
#         np.random.randint(2, size=(2,)).astype('int32')])
#     print(dummy_input[0].dtype)
#     model(dummy_input)
#     onnx_program = torch.onnx.dynamo_export(model, dummy_input)
#     onnx_program.save("agents/betatetris-v2-30hz-18f.onnx")
