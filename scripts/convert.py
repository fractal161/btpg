'''
BetaTetris is written in pytorch, which can't be run in the browser by
defeault. Instead, we convert the model to the ONNX format, where we can
leverage the ONNX Runtime to run the converted model in the browser.

Source: https://pytorch.org/tutorials/beginner/onnx/export_simple_model_to_onnx_tutorial.html
'''
import re

import numpy as np
import torch
import onnx

from models import BetaTetrisV1, BetaTetrisV2
from models.v2 import obs_to_torch
# output is rotation, y, x

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

## V1 CODE
#model = BetaTetrisV1(192, 8)
#model.load_state_dict(torch.load('agents/betatetris-v1.pth', map_location=torch.device('cpu')))
#
## see https://github.com/pytorch/pytorch/issues/99662
#model.eval()
#dummy_input = torch.randn(1, 14, 20, 10, device=torch.device('cpu')) # random input for onnx
## torch.onnx.export(model, dummy_input, "test.onnx")
#onnx_program = torch.onnx.dynamo_export(model, dummy_input)
#onnx_program.save("agents/betatetris-v1.onnx")

example_input = np.load('example_states.npz')
example_input = [example_input['arr_' + str(i)] for i in range(5)]

# V2 CODE (TODO: make this work)
# https://github.com/adrien1018/betatetris-tablebase/blob/255027f608125a41463c79599b157c5cf2fecc26/python/fceux.py#L201
with torch.no_grad():
    #state_dict = torch.load('agents/betatetris-v2-30hz-18f.pth', map_location=torch.device('cpu'))
    state_dict = torch.load('../../betatetris-tablebase/python/models/30hz-18f.pth')
    channels = state_dict['main_start.0.main.0.weight'].shape[0]
    start_blocks = len([0 for i in state_dict if re.fullmatch(r'main_start.*main\.0\.weight', i)])
    end_blocks = len([0 for i in state_dict if re.fullmatch(r'main_end.*main\.0\.weight', i)])
    model = BetaTetrisV2(start_blocks, end_blocks, channels).to(device)
    model.load_state_dict(state_dict)
    #for i in state_dict:
    #    print(i, state_dict[i].dtype)
    model.eval()

    # random input for onnx to use
    dummy_input = obs_to_torch([
        np.random.rand(3, 6, 20, 10).astype('float32'),
        np.random.rand(3, 28).astype('float32'),
        np.random.rand(3, 14, 20, 10).astype('float32'),
        np.random.rand(3, 28).astype('float32'),
        np.random.randint(2, size=(3, 2)).astype('int32')])
    print(dummy_input[0].dtype)
    model(dummy_input)
    v1 = model(obs_to_torch(example_input))
    onnx_program = torch.onnx.dynamo_export(model, dummy_input, export_options=torch.onnx.ExportOptions(dynamic_shapes=True))
    #print(onnx_program.model_proto.functions)
    #print('-------')
    #print(onnx_program.model_proto.graph)
    #print('-------')
    #print(onnx_program.model_proto.opset_import)
    #print(onnx.helper.printable_graph(onnx_program.model_proto.graph))
    onnx_program.save("agents/betatetris-v2-30hz-18f.onnx")


from onnxruntime import InferenceSession

# Path to ONNX model
onnx_model_path = "agents/betatetris-v2-30hz-18f.onnx"
sess = InferenceSession(onnx_model_path)
v2 = sess.run(None, {
    i: example_input[x]
    for x, i in enumerate(['board', 'board_meta', 'moves', 'moves_meta', 'meta_int'])
})
print(np.nan_to_num(np.abs(v1[0].cpu().numpy() - v2[0])).max())
print(np.abs(v1[1].cpu().numpy() - v2[1]).max())
