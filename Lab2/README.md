# Statistical Criteria for Open Text

**Objective**: The purpose of this work is to master statistical methods for distinguishing meaningful text from random sequences, comparing them, and determining errors of the first and second kind.

**Main Tasks**:
1. On a large Ukrainian text (>1MB), where:
   - the letter "ґ" is replaced with the letter "г";
   - the apostrophe symbol and all other special characters in the text, including spaces, are removed;
   - the text contains only lowercase alphabet letters.
   It is necessary to calculate the frequencies of letters and bigrams, as well as entropy and index of coincidence.

2. Obtain $N$ texts $X$ in Ukrainian with lengths $L = 10$, $100$, $1000$, and $10000$, for each of which distorted texts $Y$ are generated. The number $N$ is determined according to the table:
   \begin{table}[ht]
       \centering
       \begin{tabular}{|c|c|}
           \hline
           $L$ & $N$ \\
           \hline
           10 & 10000 \\
           100 & 10000 \\
           1000 & 10000 \\
           10000 & 1000 \\
           \hline
       \end{tabular}
   \end{table}
   Text distortion is performed using the following methods:
   - by applying the Vigenère cipher with a random key of length $r=1,5,10$:
     $y_i=(x_i+Key_{(i mod r)})mod(m)$;
   - by applying the affine and affine bigram substitution ciphers with random keys $a,b\in (Z_m)^l$:
     $y_i=(a\cdot x_i+b)mod(m^l)};
   - $y_i$ is a uniformly distributed sequence of symbols from $(Z_m)^l$;
   - $y_i$ is calculated according to the following relationship:
     $y_i=(s_{i-1} + s_{i-2})mod(m^l)$, where $s_0,s_1\in_R (Z_m)^l$.

3. Implement the $l$-gram criterion and its variations (2.0-2.3 in the code), the coincidence index criterion (4.0 in the code), the empty box criterion (5.0 in the code), and the structural criterion (using the Deflate compression algorithm of the .NET platform); test their performance on generated $N$ texts for each length $L$. Calculate the probabilities of errors of the first and second kind.
   All mentioned criteria (and other formulas) that use the value $l$ should take values $l = 1$ and $l = 2$, thus implementing the unigram and bigram criteria.

4. Generate a random text of length $L = 10000$, which is definitely not coherent text in Ukrainian (for example, a text consisting of a huge number of letter "а": <<aaaaaaaa...>>). Apply one of the distortion variants (of choice) to this text, then apply one of the implemented criteria (of choice). Compare the results of applying the criterion to different texts.

**Additional Tasks**:
1. Implement structural criteria using:
   - the GZip compression algorithm embedded in the .NET platform;
   - the Deflate compression algorithm embedded in the .NET platform;
   - David Huffman coding;
   - Shannon–Fano coding;
   - the LZ77 compression algorithm (Lempel–Ziv);
   - arithmetic coding;
   - byte version of the RLE (run-length encoding) compression algorithm;
   - bit version of the RLE compression algorithm.
   
2. Compare the results of the obtained modifications of the structural criterion with the results of the previous criteria.


# Progress of Work, Description of Challenges and Their Solutions

During the implementation of the work, no particular difficulties arose, but the work is quite extensive, so it required some time to understand what needs to be done, as well as its implementation. Among the difficulties, choosing parameters when constructing one criterion or another can be highlighted, as it relied on intuitive approach and experiments.

One of the interesting aspects for me was the study of data compression algorithms. I learned that there are algorithms with and without lossy compression. Algorithms of the first type are usually used for compressing audio, video, and images (for example, in computer vision), because lower quality in such cases does not interfere with understanding the main information and is sometimes disregarded. However, when it comes to text data, every byte of information is important, because, for example, neglecting three bytes in the message "Do not lose Vasily" can change it to the opposite meaning. The most popular compression algorithms used in modern generators are Huffman and LZW (Lempel-Ziv-Welch). There are also the Shannon-Fano algorithm, arithmetic coding, RLE (run-length encoding), KWE (keyword encoding), and LZ (Lempel-Ziv) algorithms.

I first learned about the Huffman algorithm at one of Oleg Cherny’s last lectures on the algorithm analysis fundamentals, so I wanted to recall the main idea of this algorithm and later implement it. After that, I decided to implement the Shannon-Fano algorithm, as it is similar to the Huffman algorithm. After some time, I wanted to understand the LZ77 algorithm, arithmetic coding, byte and bit versions of the RLE algorithm, which were implemented towards the end of the work. Additionally, the Deflate and GZip algorithms of the .NET platform were applied for implementing the structural criterion.

# Structural Criterion Description

1. Generate a random sequence $Z$ of length $L$ from the alphabet $Z_m$, where $m=32$ (the number of letters in the Ukrainian alphabet with the substitution of the letter "ґ" with "г").
2. Apply the data compression algorithm \textit{Algorithm} to the distorted open text $Y$ and the sequence $Z$ of length $L$, obtaining sequences of lengths $L_Y$ and $L_Z$ respectively.
3. If $|L_Y-L_Z|\leqslant C$, then the sequence is random; otherwise, it is not ($C$ is a certain constant that may vary depending on the chosen \textit{Algorithm}).

It is important to note that in the work, the pseudo-random generator \textit{rand} of C++ is used, and the compression algorithms \textit{Deflate} and \textit{GZip} proposed by Microsoft in the System.IO.Compression namespace for .NET projects are chosen. In addition, implemented compression algorithms include \textit{Huffman}, \textit{Shannon-Fano}, \textit{LZ77}, \textit{arithmetic coding}, byte and bit versions of \textit{RLE}. The formulation of the third point as a comparison of the absolute difference in the length of the compressed distorted text $Y$ and the compressed random text $Z$ with a certain constant arises from the descriptions of the compression algorithms themselves, the basis of which is the idea of using statistics (letter distribution) on input data to reduce their size. Considering that the entropy of meaningful text is minimal, and random text is maximal, it is appropriate to consider $|L_Y-L_Z|$, since it is easier to compress open text (lower entropy results in shorter length). Let us consider the structure of the chosen compression algorithms.

## Compression Algorithms Description

### Deflate (1996)

The Deflate algorithm is a data compression algorithm proposed by Phil Katz for archiving files in the ZIP format. This algorithm utilizes a combination of the LZSS (Lempel–Ziv–Storer–Szymanski, 1982) and Huffman (1952) algorithms. LZSS is a compression algorithm derived from LZ77 (Lempel–Ziv, 1977), where statistical dependencies between letters in a message are considered for data compression, meaning the algorithm can determine which words occur more frequently than others. The idea of the algorithm is that if a certain set of letters is encountered repeatedly in the input stream, the output stream contains only references to them. The Huffman algorithm takes into account the probability distribution of letters in the message and constructs the corresponding binary tree. The .NET platform allows using the ready-made Deflate compression algorithm, so implementation details are omitted.

### GZip

GZip is essentially a Microsoft development, as the company notes that the compression algorithm used in GZip is based on Deflate, but the presentation of the result is not in the dfl format, but in gz, which allows opening the compressed file with archivers like WinZip. Additionally, GZip includes CRC (cyclic redundancy check), a code that calculates the checksum for data integrity verification, which is important when transmitting a large amount of information over an unprotected communication channel without error correction protocol (such as transmitting data over raw sockets or UDP). Consequently, GZip operates slower than Deflate.

### Brotli (2015)

The Brotli algorithm was first proposed by Google for compressing web fonts in 2015, and a new version was released in August 2020. Today, Brotli is considered the best data compression algorithm because it can compress 20% more information on the network than GZip, thereby speeding up website loading in browsers that use Brotli. This algorithm is based on a combination of LZ77, the Huffman algorithm, and second-order context modeling.

### Shannon-Fano (1950)

The Shannon-Fano algorithm is one of the first algorithms that provided minimal-length codes, and its idea is to use non-uniform symbol encoding. This algorithm assigns short binary codes to more frequently encountered letters and longer codes to less frequent ones. For a better understanding of this algorithm, let us consider it with an example.

**Example:**
1. *Calculate the probabilities of occurrences of each letter in the text.*

    Suppose we have the text "hellohello", then the alphabet will be *A={h,e,l,o}*, and the probability vector $P=(p_{h}, p_{e}, p_{l}, p_{o}) = (\frac{1}{5},\frac{1}{5},\frac{2}{5},\frac{1}{5})$.

2. *Build a binary tree using the approach of solving the partition problem, where the goal is to divide the set into two subsets so that the weights of both subsets are as evenly balanced as possible.*

    In our example, initially we have two such subsets: $\{l\}$ and $\{h,e,o\}$. Then we apply the partitioning again, but only to the second subset, since in the first one we have only one letter and there is nothing to partition. Then we build a binary tree, where we assign 0 to the left branch and 1 to the right branch.

3. *Moving through the obtained tree, assign binary codes to the letters. Write the initial text in binary representation according to Shannon-Fano.*

    For "hellohello" we get the following codes:
    - $h=<10>$,
    - $e=<110>$,
    - $l=<0>$,
    - $o=<111>$.
    Therefore, for "hellohello" we have 101100111101100111.

Below you can see the LaTeX code for generating the figure for this example. You can copy this code and use it in a LaTeX document to reproduce the figure.
   
```latex
\begin{figure}
\centering
\begin{tikzpicture}[level distance=1.3cm,
level 1/.style={sibling distance=5cm},
level 2/.style={sibling distance=3cm}]
\tikzstyle{every node}=[rectangle,draw]
\node (Root) {$\{h,e,l,o\}$}
    child { node {$\{l\}$} edge from parent node[left,draw=none] {0} }
child {
    node{$\{h,e,o\}$}
    child { node {$\{h\}$} edge from parent node[left,draw=none] {0} }
    child {
    node {$\{e,o\}$}
    child { node {$\{e\}$} edge from parent node[left,draw=none] {0} }
    child { node {$\{o\}$} edge from parent node[right,draw=none] {1} }
    edge from parent node[right,draw=none] {1}
    }
    edge from parent node[right,draw=none] {1}
};
\end{tikzpicture}
\caption{Binary tree for the Shannon-Fano algorithm, created using the tikz package and the trees library.}
\label{pic_SF}
\end{figure}
```

The figure looks like:

![Binary Tree for Shannon-Fano Algorithm](https://drive.google.com/file/d/1xGUSHGOc-yq7Jod7QCa-MRMnfhdAvhBH)

### Huffman (1952)

It is interesting to note that David Huffman came up with his algorithm during his student years at MIT while working on his thesis under Robert Mario Fano. This very algorithm is still used in most compression algorithms today because it remains one of the most optimal methods for symbolic coding. To better understand this algorithm, let us consider it with an example.

**Example:**
1. *Calculate the probabilities of occurrences of each letter in the text.*

   Let us say we have the text "hellohello", then the alphabet will be *A={h,e,l,o}*, and the probability vector $P=(p_{h}, p_{e}, p_{l}, p_{o}) = (\frac{1}{5},\frac{1}{5},\frac{2}{5},\frac{1}{5})$.

2. *Build a binary tree, starting from the leaves (where the vertices contain letters from set A), by merging vertices with the smallest weight (probability) into a common one with a weight equal to the sum of the weights of the previous two vertices.*

  For the text "hellohello," we get the binary tree as shown in the LaTeX code below, where 0 is assigned to the left branch and 1 to the right branch.

3. *Traverse the obtained tree to assign binary codes to letters. Write the initial text in its binary representation according to Huffman (starting from the root of the tree).*

Below you can see the LaTeX code for generating the figure for this example. You can copy this code and use it in a LaTeX document to reproduce the figure.

```latex
\begin{figure}
\centering
\begin{tikzpicture}[grow'=up,
level distance=1.3cm,
text width=0.25cm, align=center,
level 1/.style={sibling distance=4cm},
level 2/.style={sibling distance=3cm}]
\tikzstyle{every node}=[rectangle,draw]
\node (Root) {1}
    child {
    node{$\frac{2}{5}$}
    child { node {$\frac{1}{5}$} 
    child { node {h} edge from parent node[left,draw=none] {0}}
    edge from parent node[left,draw=none] {0}
    }
    child {
    node {$\frac{1}{5}$}
    child { node {e} edge from parent node[right,draw=none] {1} }
    edge from parent node[right,draw=none] {1}
    }
    edge from parent node[left,draw=none] {0}
    }
child {
    node{$\frac{3}{5}$}
    child { node {$\frac{2}{5}$} 
    child { node {l} edge from parent node[left,draw=none] {0}}
    edge from parent node[left,draw=none] {0}
    }
    child {
    node {$\frac{1}{5}$}
    child { node {o} edge from parent node[right,draw=none] {1} }
    edge from parent node[right,draw=none] {1}
    }
    edge from parent node[right,draw=none] {1}
};
\end{tikzpicture}
\caption{Binary tree for the Huffman algorithm, created using the tikz-qtree package and the trees library.}
\label{pic_Huff}
\end{figure}
```
The figure looks like:

![Binary Tree for Huffman Algorithm](https://drive.google.com/uc?id=156jhC9zDx92wVPcMFgOpkC5v-mk3SC23)

For "hellohello," we get the following codes:
- $h=<000>$,
- $e=<011>$,
- $l=<100>$,
- $o=<111>$.

Thus, for "hellohello," we have 000011100111000011100111.

**Remark:**
It is important to note that equivalents can be constructed differently, as it depends on how we decided to divide the initial set for tree construction (Shannon-Fano algorithm) or which vertices we decided to merge (Huffman algorithm).

**Remark:**
The Shannon-Fano algorithm is not used in practice because, in general, the set partition problem is NP-complete, and the proposed solution in point 2 is pseudo-polynomial, meaning that such an algorithm works efficiently only when we have a small sample.

**Remark:**
The Shannon-Fano and Huffman algorithms are greedy, which means that the optimal result is not always achievable. Furthermore, for any compression algorithm, there exists a data sequence for which the algorithm provides the best possible compression, and vice versa, for any compression algorithm, there exists a data sequence for which the algorithm provides the worst possible compression. In the provided example, compression of the message "hellohello" is better with Shannon-Fano than with Huffman, although the former algorithm is not used in practice.

**Remark:**
One of the reasons why the Huffman algorithm may not provide the best compression is the low entropy of the source (usually less than one), meaning that if less than one bit is allocated per letter, for example, when some letters occur very often and others very rarely, and the overall text is very small.

**Remark:**
The set partition problem can be considered as a special case of the subset sum problem, which involves finding a non-empty subset in a multiset such that the sum of all its elements is equal to zero.

**Remark:**
Comparing the Huffman and Shannon-Fano algorithms, we can see that they are similar, but the main difference lies in the second step, where in the Huffman algorithm, the binary tree is built starting from the tree's leaves, and in the Shannon-Fano algorithm, it starts from the root, which on large data requires an exponential number of computations compared to the Huffman algorithm.

#### LZ77 (1977)

A good resource for data compression is not only the frequency of letters but also the frequency of words - this idea is implemented in the Lempel-Ziv algorithms. Let us consider the LZ77 algorithm.

Let us say we have a data stream that we move along using a certain window, which consists of two parts: the right part corresponds to the chunk of text we are analyzing, and the left part corresponds to the chunk that we can view to search for word repetitions and then reference them.

**Example:**
Consider the algorithm using the example of compressing the text "hellohello." To do this, we read the characters sequentially until they start repeating. So, at the first stage, we get "hel(1,1)," where (1,1) is a reference to the letter "l," where the first 1 corresponds to the number of steps we need to take back from the current state to reach the letter "l," and the second 1 corresponds to the number of characters to take starting from the letter "l." Applying the algorithm further, we get "hello(5,5)," where (5,5) is a reference to "hello" (meaning the algorithm takes 5 steps back and takes 5 characters in a row).

**Remark:**
It is important to note that we have limited memory, so we cannot view data that occurred a long time ago (our window is limited). Thus, by reducing the size of the window, we lose the degree of possible data compression and vice versa. In practice, a window covering up to 256 characters is sufficient.

**Remark:**
There are other versions of the Lempel-Ziv algorithms. For example, unlike LZ77, in LZ78, we only have the right part of the window, and the left part is replaced by a dictionary to which we add words and assign them a certain digital equivalent.

### Arithmetic coding (IBM, 1976)

In arithmetic coding, the input data stream is viewed as the representation of a certain "number," for which an attempt is made to associate a certain interval from the segment $[0,1]$ with each unique symbol of the text.

- *Add an end-of-text symbol to the end of the message.*

We obtain the message "hellohello!", where "!" is the end-of-text symbol.

- *Calculate the probabilities of encountering each symbol in the text.*

In this example, we have the alphabet $A=\{h,e,l,o,!\}$ and the probability vector $P=(p_h,p_e,p_l,p_o,p_!)=(\frac{2}{11},\frac{2}{11},\frac{4}{11},\frac{2}{11},\frac{1}{11})$.

- *Sequentially assigning the probability values to $[0,1]$, divide the interval into parts, each corresponding to a unique symbol of the text. Then, take the first character in the text and work with the interval corresponding to it, repeating the division procedure for it. Next, work with the interval corresponding to the second character of the text, and so on. The intervals become smaller and smaller, eventually allowing us to identify the point corresponding to the end-of-text symbol.*

For the message "hellohello!", the interval $[0,\frac{2}{11}]$ corresponds to the letter "h", $[\frac{2}{11},\frac{4}{11}]$ corresponds to "e", $[\frac{4}{11},\frac{8}{11}]$ corresponds to "l", $[\frac{8}{11},\frac{10}{11}]$ corresponds to "o", and $[\frac{10}{11},1]$ corresponds to "!".

Below you can see the LaTeX code for generating the corresponding illustration. You can copy this code and use it in a LaTeX document to reproduce it.

```latex
\begin{center}
    \begin{tikzpicture}
    \draw (0,0) -- (10,0);
    \foreach \x/\xtext in {0/0,10*0.181818182/$\frac{2}{11}$,10*0.363636364/$\frac{4}{11}$,10*0.727272728/$\frac{8}{11}$,10*0.90909091/$\frac{10}{11}$,10*1/1}{
      \draw (\x cm,-2pt) -- (\x cm,2pt) node[above] {\xtext};
      }
    \foreach \x/\xtext in {5*0.181818182/h,7.5*0.363636364/e,7.5*0.727272728/l,9*0.90909091/o,10.5*0.90909091/!}{
      \draw (\x cm,-0pt) -- (\x cm,0pt) node[above] {\xtext};  
    }
\label{pic_ari}
\end{tikzpicture}
\end{center}
```
The figure looks like:

![First Iteration](https://drive.google.com/file/d/1KNdAWT_kIpi0NWx-KbFfnh36019L9pwd)

In the message, the first letter is "h", so we work with the interval $[0,\frac{2}{11}]$ and sequentially divide it into segments in the same proportions as the interval $[0,1]$, assigning the obtained parts to symbols from the alphabet.

![Second Iteration](https://drive.google.com/file/d/181NwuTt_T24Kn7Lk9DXqfZwypRuQ9R4A)

The second character in the message is "e", so we have a segment of length $\frac{4}{121}$, which is divided as follows:

![Third Iteration](https://drive.google.com/file/d/1vY05YPeO5yxZhxoHnQ2ohXMQWg_fpVJM)

We continue this procedure until we run out of characters in the text, observing which segment contains "!".

*At this step, we obtain the result of our encoding by extracting the fractional part of the point corresponding to the end-of-text symbol within the obtained interval. To determine how many digits of precision we need to consider in the fractional part, it is proposed to examine the number up to the first digit that does not match the boundaries of the interval.*

### RLE (1967)

RLE is a compression algorithm that works well on data where identical symbols occur consecutively. There are several versions of this algorithm. Let's consider two approaches to such compression: byte-based and bit-based approaches.

1. **Byte-based RLE**

   Instead of conditional 5 letters "a", one such letter is assigned to each one, along with their number, i.e., "5a". The same happens with a chain of letters (e.g., repeating words). For a sequence of non-repeating characters, writing them together with 1 (indicating that the symbol occurred once) is not a reasonable practice, as it doubles the message size. In such cases, the length of the sequence is appended to it, which is also a data expansion, but only by 1 byte. For example, for the message "abcdad", it corresponds to "6abcdad". The question arises of distinguishing between subsequences that are repeats and those that are not. The answer lies in storing another bit, which is an indicator of repetition.

   For the message "hellohello", we have compression from 80 to $5*8+8+8=56$ bits and the output text "12hello", where "1" indicates repetition and "2" indicates two repetitions.

2. **Bit-based RLE**

   Essentially, the bit version implements the same ideas as the byte version, but at the bit level, as we can consider the input sequence as a sequence of bits.

Finally, in this section, it is worth mentioning Shannon's theorem on optimal coding because this is how the maximum possible degree of data compression can be calculated. Also, from this theorem, it becomes clear why the entropy criterion is the most powerful and best.

*Theorem:*
There is a possibility to create a system of optimal coding for discrete messages, for which the average number of binary numbers per symbol of the input message asymptotically tends to the entropy of the message source.

### Conclusions

Here, I worked on various statistical methods for distinguishing random text from meaningful text: criteria for prohibited/frequent $l$-grams and their variations, entropy criterion, criterion through calculating the coincidence index, empty box criterion, and structural criterion. The work involved testing 7 methods on texts of different lengths $L$. It has been shown that for $L=1000$ and $L=10000$, as well as a large number of generations $N$, all methods work very well, i.e., the first and second type errors are minimal, and the best ones are the criteria through calculating the coincidence index, the empty box criterion, and the structural criterion. The criteria 2.0-2.3 performed the worst because they take into account the presence of the most frequent letters in texts and in the absence of one of them, they give an answer that the text is random. This is particularly noticeable in short sequences; for example, criteria 2.0-2.2 identified the sequence "хлопчикщов" as random simply because the letter "а" is missing.

In addition, research was conducted on various data compression algorithms. Implemented algorithms include Huffman, Shannon-Fano, LZ77, byte and bit versions of RLE, Deflate, GZip, and arithmetic coding method. It was found that the best results are achieved by the Deflate and GZip algorithms, which is not surprising since they use a combination of the Huffman and LZ77 algorithms. The Shannon-Fano algorithm performs the worst on large texts, but not on the shortest ones. The byte and bit versions of RLE compress distorted and random data well. Interestingly, the bit version provides a more accurate result. With text lengths of $L=10000$, a number of iterations $N=1000$, and a set parameter $k=10$ (for the Vigenère cipher), the first and second type errors for the structural criterion coincide and are very good (0 and 0.023). Also I have made an attempt to use the Brotli and ZLib compression algorithms of the .NET platform, which showed that there are errors in the Microsoft documentation, the most important of which is the lack of the ability to use the built-in Brotli and ZLib when implementing .NET projects in C++.

If we highlight only one criterion among all proposed in the laboratory and considered independently, then in my opinion, the entropy criterion is the most powerful because, recalling Shannon's theorem on optimal coding, entropy gives an idea of the maximum possible compression of certain information. For example, structural criteria use compression algorithms that can both compress and inflate data. Accordingly, the question arises of constructing such a compression algorithm (archiver) that would take into account which algorithm would give the best results on certain data. Unfortunately, no one has implemented such an archiver yet, and if there were such an archiver, it would work for a very long time.
