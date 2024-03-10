'''
BetaTetris is written in pytorch, which can't be run in the browser by
defeault. Instead, we convert the model to the ONNX format, where we can
leverage the ONNX Runtime to run the converted model in the browser.

Source: https://pytorch.org/tutorials/beginner/onnx/export_simple_model_to_onnx_tutorial.html
'''

import torch
from model import Model
# output is rotation, y, x

import torch.nn as nn
import torch.nn.functional as F


class MyModel(nn.Module):

    def __init__(self):
        super(MyModel, self).__init__()
        self.conv1 = nn.Conv2d(1, 6, 5)
        self.conv2 = nn.Conv2d(6, 16, 5)
        self.fc1 = nn.Linear(16 * 5 * 5, 120)
        self.fc2 = nn.Linear(120, 84)
        self.fc3 = nn.Linear(84, 10)

    def forward(self, x):
        x = F.max_pool2d(F.relu(self.conv1(x)), (2, 2))
        x = F.max_pool2d(F.relu(self.conv2(x)), 2)
        x = torch.flatten(x, 1)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x


model = Model(192, 8)
model.load_state_dict(torch.load('models/betatetris.pth', map_location=torch.device('cpu')))

# very relevant issue: https://github.com/pytorch/pytorch/issues/99662
model.eval()
dummy_input = torch.randn(1, 14, 20, 10, device=torch.device('cpu')) # random input for onnx
# for some reason the dynamo_export throws an error but this one doesn't
# we will see if it actually worked correctly
# torch.onnx.export(model, dummy_input, "test.onnx")

# model = MyModel()
# dummy_input = torch.randn(1, 1, 32, 32) # random input for onnx
# 
onnx_program = torch.onnx.dynamo_export(model, dummy_input)
onnx_program.save("models/betatetris.onnx")
