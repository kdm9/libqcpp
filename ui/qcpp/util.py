# Copyright 2015 Kevin Murray <spam@kdmurray.id.au>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import jinja2
from jinja2 import Environment, PackageLoader
import mpld3.urls

def pothole2title(string):
    words = string.split('_')
    return " ".join([w.title() for w in words])

def nice_params(param_dict):
    nice = {}
    for k, v in param_dict.items():
        nice[pothole2title(k)] =  v
    return nice

def include_file(name):
    return QCPP_LOADER.get_source(QCPP_ENV, name)[0]

QCPP_LOADER = PackageLoader('qcpp', 'templates')
QCPP_ENV = Environment(loader=QCPP_LOADER)

with open(mpld3.urls.D3_LOCAL) as fh:
    D3 = fh.read()

with open(mpld3.urls.MPLD3_LOCAL) as fh:
    MPLD3 = fh.read()

QCPP_ENV.globals['include_file'] = include_file
