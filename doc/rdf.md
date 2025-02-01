inputs (required List[InputTensor]) Describes the input tensors expected by this model.

(InputTensor) is a Dict with the following keys:

axes (Axes→String) Axes identifying characters from: bitczyx. Same length and order as the axes in shape.

character	description
b	batch (groups multiple samples)
i	instance/index/element
t	time
c	channel
z	spatial dimension z
y	spatial dimension y
x	spatial dimension x
data_type (String) The data type of this tensor. For inputs, only float32 is allowed and the consumer software needs to ensure that the correct data type is passed here. For outputs can be any of float32, float64, (u)int8, (u)int16, (u)int32, (u)int64. The data flow in bioimage.io models is explained in this diagram.

BioImage.IO Data Flow
name (String) Tensor name. No duplicates are allowed.

shape (Union[ExplicitShape→List[Integer] | ParametrizedInputShape]) Specification of input tensor shape.

(ExplicitShape→List[Integer]) Exact shape with same length as axes, e.g. shape: [1, 512, 512, 1]
(ParametrizedInputShape) A sequence of valid shapes given by shape = min + k * step for k in {0, 1, ...}. ParametrizedInputShape is a Dict with the following keys:
min">min (List[Integer]) The minimum input shape with same length as axes
step">step (List[Integer]) The minimum shape change with same length as axes
data_range (Tuple) Tuple (minimum, maximum) specifying the allowed range of the data in this tensor. If not specified, the full data range that can be expressed in data_type is allowed.

preprocessing (List[Preprocessing]) Description of how this input should be preprocessed.

(Preprocessing) is a Dict with the following keys:
name">name (String) Name of preprocessing. One of: binarize, clip, scale_linear, sigmoid, zero_mean_unit_variance, scale_range.
kwargs">kwargs (Kwargs→Dict[String, Any]) Key word arguments as described in preprocessing spec.
license (required String) A SPDX license identifier(e.g. CC-BY-4.0, MIT, BSD-2-Clause). We don't support custom license beyond the SPDX license list, if you need that please send an Github issue to discuss your intentions with the community.

name (required Name→String) Name of this model. It should be human-readable and only contain letters, numbers, underscore '_', minus '-' or spaces and not be longer than 64 characters.

test_inputs (required List[Union[URI→String | Path→String]]) List of URIs or local relative paths to test inputs as described in inputs for a single test case. This means if your model has more than one input, you should provide one URI for each input.Each test input should be a file with a ndarray in numpy.lib file format.The extension must be '.npy'.

test_outputs (required List[Union[URI→String | Path→String]]) Analog to test_inputs.

timestamp (required DateTime) Timestamp of the initial creation of this model in ISO 8601 format.

weights (required Dict[String, Union[KerasHdf5WeightsEntry | OnnxWeightsEntry | PytorchStateDictWeightsEntry | TensorflowJsWeightsEntry | TensorflowSavedModelBundleWeightsEntry | TorchscriptWeightsEntry]])
