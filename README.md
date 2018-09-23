<h1>Question-guided Search Engine</h1>

Source code of QUSE (<b>QU</b>estion-guided <b>S</b>earch <b>E</b>ngine, a search engine that interacts with a user by asking quesitons to refine an initial keyword query. More details are in the paper by A. Kotov and C. Zhai "Towards natural question guided search", Proceedings of the 19th International Conference on World Wide Web, 2010. Please cite this paper, if you are using any part of this project.

<h2>Requirements:</h2>
<ul>
  <li>Minipar or any other dependency parser</li>
  <li>FastCGI</li>
  <li>MySQL</li>
</ul>

<h2>Project directories:</h2>

<ul>
  <li><b>distr</b>: project distribution (compiled binaries and config files) for debug and release configurations</li>
  <ul>
    <li>bin/parser</li>binary code of Minipar dependency parser
    <li>etc/minipar</li>data for Minipar
    <li>etc/quse.conf</li>main configuration file of QUSE
    <li>etc/schema.sql</li>scheme of the database for index and search sessions 
  </ul>
  <li><b>include</b>: incldue files</li>
  <li><b>index</b>: index constuction</li> 
  <li><b>indio</b>: index input/output API</li>
  <li><b>qproc</b>: search results generation and SERP rendering</li>
  <li><b>sbox</b>: search box presentation and query processing</li> 
</ul>

<h2>Compilation</h2>

Set PROJ_DIR variable in Makefile.config to the project directory path and make.


