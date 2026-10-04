#include "SimEnv.h"
#include "StockParser.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(backtest_env, m) {
    m.doc() = "Step-wise view of the C++ backtesting engine for Gymnasium";

    m.def("read_csv", &ReadCsv, py::arg("filename"), "Read closing prices from a Yahoo Finance CSV");

    py::class_<SimEnv>(m, "SimEnv")
        .def(py::init<std::vector<double>, double, double>(), py::arg("prices"),
             py::arg("starting_cash") = 10000.0, py::arg("cost_rate") = 0.0)
        .def("reset", &SimEnv::Reset, py::arg("start"), py::arg("end"))
        .def("step",
             [](SimEnv &env, int action) {
                 StepResult r = env.Step(action);
                 return py::make_tuple(env.Observe(), r.reward, r.done, r.portfolioValue);
             },
             py::arg("action"))
        .def("observe", &SimEnv::Observe)
        .def("num_bars", &SimEnv::NumBars)
        .def_static("num_features", &SimEnv::NumFeatures)
        .def_static("warmup", &SimEnv::Warmup);
}
