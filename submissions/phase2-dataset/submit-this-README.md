# README

This is a document describing how we created the dataset, how to run the scripts to recreate the dataset and the task split up
between the team for the project. We strongly recommend using a Markdown previewer to read this, specially for the instructions to
recreate the dataset: either your editor's markdown preview or [an online previewer](https://dillinger.io/).

## Dataset description

Our dataset is derived from [Allstar](https://allstar.jhuapl.edu), a public repository of Debian packages specially created
for reverse engineering research.

The dataset is a JSON file which contains source level information about functions present in each package(the name of the source
file and the line numbers in the source file). We split it up into 1 JSON file for each package.

The dataset is created using 3 levels of filtering:

1. **Debian tags**: [Debian tags](https://wiki.debian.org/Debtags/FAQ) is a collection of metadata for each Debian package. While
   the actual vocabulary is quite detailed and extensive, we only seek to obtain the following information in the first level of
   classication:
   - What programming languages are used in the package?
   - Does the package contain a runnable program?

   Based on this information, we classify packages into following categories:

    1. Packages that don't contain a runnable program
    2. Package which may or may not contain a runnable program
    3. Runnable packages written in C
    4. Runnable packages with no C code at all
    5. Runnable packages partially written in C and partially some other language(s)
    6. Runnable packages whose programming language is unknown

    - Categories 1 and 4 are directly rejected because we are focusing only on C programs and our evaluation requires executables
      since we plan to run test suites to ensure semantic correctness is preserved.
    - Categories 2, 5 and 6 require further processing. For now, we ignore 2 and perform the second filtering for other
      categories.
    - Category 3 is directly shortlisted for the final filtering.

2. **Linguist**: In this step, we use [GitHub's linguist](https://github.com/github/linguist) to determine the most common
   programming language in a package. If the most common language in the package is C, then we shortlist the package for the third
   level filtering.
3. **Allstar**: In this final filtering, we check the Allstar repository to see if there is an AMD64 executable present for the
   package. If there is none present, then we discard the package and do not add it to the dataset.

After this filtering, we have a collection of functions as well as the executables for the dataset. We have a total of 646729 functions
across 1146 packages. Since our dataset would be extremely large(with both source files and binary executables), we are including
only the source level information of functions(compressed) derived from the source packages and bianries, which can be easily
obtained from the public Allstar and Debian source package repositories.

We also expect additional filtering and processing of the dataset would become necessary as we make further progress on the
project and more aspects become clearer. We will add additional details in future updates about the project.

## Task split up

For the project, we split our initial tasks along these three categories:

1. Tokenizing source code and IDA decompiler output.
2. Creating and training the model using fairseq.
3. Creating dataset.

We assigned a specific person responsible for each task who will perform the implementatin. However, high level details of each
task are discussed among the team and finalized. Additionally, members also help the responsible person as needed. The task
responsibility assignment was done as follows:

- William Gibbs: Tokenizing source code using IDA decompiler output.
- Yihui Zheng: Tokenizing source code using clang and IDA decompiler output.
- Maxfield Lehman: Responsible for model creation and training using fairseq. Also helped with generating dataset.
- HuiJun Tay: Responsible for model creation and training using fairseq. Also helped with generating dataset.
- Arvind Sriram Raj: Creating initial dataset from which training and testing sets will be derived.

In addition, we will split the final evaluation task amongst all of us us. We have high level evaluation tasks identified but
since specific details of those tasks have not yet been identified, we do not have a clear distribution of tasks amongst us yet.

## How to run the code

1. Setup a python virtual environment and install packages required for running the code. We use Ubuntu 18.04 LTS and the
   instructions are mostly based on that; please adjust accordingly for other distros:

   ```bash
   $ sudo apt update && sudo apt install python3-pip
   $ sudo pip install virtualenvwrapper
   # Add "source /usr/local/bin/virtualenvwrapper.sh" to your shell's RC($HOME/.bashrc, $HOME/.zshrc etc) file
   # and restart terminal before continuing
   $ mkvirtualenv --python=`which python3` cse576
   (cse576) $ pip install -r requirements.txt
   ```

2. Parse debtags file into a TinyDB database:

    ```bash
    (cse576) $ python3 parse-debtags-to-tinydb.py --debtags-file debtags-2020-10-16.txt --json debtags-2020-10-16.json
    ```

3. Perform first classification of packages using the `debtags` file.

    ```bash
    (cse576) $ python3 classify-packages-using-debtags.py --db debtags-2020-10-16.json --output-dir debtags-classification
    ```

4. Download all source packages needed. Since we download packages from Debian jessie's source repositories, this requires
   using a Debian jessie distribution to download the packages. The easiest way is to use a docker container running Debian
   jessie. As first step, [install docker using the official instructions](https://docs.docker.com/engine/install/ubuntu/).

   (Warning: This process will take a lot of time - about 11GB of packages need to be downloaded. Consider downloading just a few
    packages in each category for testing)

   ```bash
   $ docker run -v $PWD/debtags-classification:/dataset-packages --name cse576-package-source-downloader -it debian:jessie bash
   # This will pull docker the image if doesn't exist and then drop you into a shell inside the container.
   # For here on, everything is inside the container.
   $ echo "deb-src http://deb.debian.org/debian jessie main" >> /etc/apt/sources.list
   $ echo "deb-src http://security.debian.org/debian-security jessie/updates main" >> /etc/apt/sources.list
   $ echo "deb-src http://deb.debian.org/debian jessie-updates main" >> /etc/apt/sources.list
   $ apt update
   $ cd /dataset-packages
   $ mkdir runnable-c-only-packages  runnable-partially-c-packages  runnable-unknown-language-packages
   $ cd /dataset-packages/runnable-c-only-packages
   $ for i in $(cat /dataset-packages/runnable-c-only-packages.txt)
     do
        echo -n "Downloading sources of $i..."
        apt source $i
        echo "done."
     done
    $ cd /dataset-packages/runnable-partially-c-packages
    $ for i in $(cat /dataset-packages/runnable-partially-c-packages.txt)
      do
        echo -n "Downloading sources of $i..."
        apt source $i
        echo "done."
      done
    $ cd /dataset-packages/runnable-unknown-language-packages
    $ for i in $(cat /dataset-packages/runnable-unknown-language-packages.txt)
      do
        echo -n "Downloading sources of $i..."
        apt source $i
        echo "done."
      done
    # Exit from container
    $ exit
   ```

5. Perform second level classification using GitHub linguist.

    ```bash
    # Docker creates all files and folders as owned by root unless configured otherwise. Let's change that.
    $ sudo chown $USER:$USER -R debtags-classification
    $ workon cse576  # Activate virtual environment if not active
    (cse576) $ python3 filter-packages-using-github-linguist.py --packages-dir $(realpath ./debtags-classification/runnable-partially-c-packages) \
                        --output ./debtags-classification/runnable-partially-c-packages-linguist-filtered.txt
    (cse576) $ python3 filter-packages-using-github-linguist.py --packages-dir $(realpath ./debtags-classification/runnable-unknown-language-packages) \
                        --output ./debtags-classification/runnable-unknown-language-packages-linguist-filtered.txt
    ```

6. Extract data using clang parser. This step would display errors that source packages could not be found for some packages. This
   is because we use a heuristic to find the source folder(search for folders named `src`, `Src`, `source` etc). We will explore
   improving this going ahead. Alternatively, you can manually specify the source directory name using `--src-dirs`.

    ```bash
    $ workon cse576  # Activate virtual environment if not active
    (cse576) $ mkdir clang-filtered
    (cse576) $ for i in $(ls debtags-classification/runnable-c-only-packages)
               do
                    python3 parse-package-using-clang.py --package-tree $(realpath ./debtags-classification/runnable-c-only-packages/$i) \
                        --output-file clang-filtered/$i.json
               done
    (cse576) $ for i in $(cat debtags-classification/runnable-partially-c-packages.txt)
               do
                    python3 parse-package-using-clang.py --package-tree $(realpath ./debtags-classification/runnable-partially-c-packages/$i) \
                        --output-file clang-filtered/$i.json
               done
    (cse576) $ for i in $(cat debtags-classification/runnable-unknown-language-packages-linguist-filtered.txt)
               do
                    python3 parse-package-using-clang.py --package-tree $(realpath ./debtags-classification/runnable-unknown-language-packages/$i) \
                        --output-file clang-filtered/$i.json
               done
    ```

7. Perform final filtering using the Allstar dataset.
   NOTE: allstar.py must be in the same directory as allstar-filter-nobins.py
         the mapping file runnable-c-only-packages-names.txt must be in the same directory
         as runnable-c-only-packages
    
    ```bash
    $ workon cse576  # Activate virtual environment if not active
    (cse576) $ python3 allstar-filter-nobins.py debtags-classification/runnable-c-only-packages
    (cse576) $ python3 allstar-filter-nobins.py debtags-classification/runnable-unknown-language-packages
    ```