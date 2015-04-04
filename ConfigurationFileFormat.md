The AIM-C configuration file is a simple text file in the format parsed by [simpleini](http://code.jellycan.com/simpleini/).

The file describes a tree of modules, with the output of each module feeding into zero, one or more other modules. Each node of the tree is an _instance_ of an AIM-C module. Each node can have a different set of parameters.

## Example ##

```
module1.name = FileInput
module1.id = file_input
module1.child1 = PZFCFilterbank

module2.name = PZFCFilterbank
module2.id = pzfc
module2.child1 = NAP

module3.name = NAP
module3.id = hcl
module3.child1 = Strobes

module4.name = Strobes
module4.id = local_max
module4.parameters = <<<ENDPARAMS
ENDPARAMS
module4.child1 = SAI

module5.name = SAI
module5.id = weighted_sai
module5.child1 = SAIMovie

module6.name = SAIMovie
module6.id = graphics_time
module6.parameters = <<<ENDPARAMS
graph.y.min=0
graph.y.max=0.5
graph.freq.label=Cochlear Channel
graph.x.label=Time Interval
graph.type=line
ENDPARAMS
```

Each module has a number. The varaibles `module<n>.name and module<n>.id` are required for each module. If `module<n>.parameters and module<n>.child<m>` are specified they will be used. If no parameters are specified for a given module, the defaults are used. Any parameters given for a module override the defaults built in to the module.

### Module names and identifiers ###
Each module specified in the tree is an instance of an AIM-C module with a specific set of parameters. By convention each instance of a module is identified by a [camel case](http://en.wikipedia.org/wiki/CamelCase) name, and each module is identified by a lowercase name with underscores.

### Module parameters ###

All AIM-C modules can have a default set of parameters. These can be overridden by parameters in the `module<n>.parameters` section of the configuration file.

