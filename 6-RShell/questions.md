1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

**Ans.** The remote client knows the output is fully received when the server sends an explicit termination signal (e.g., a special delimiter like `\n\n` or `EOF`). Alternatively, it can determine this by closing the connection. Techniques to handle partial reads include buffering data until the expected delimiter is found, using a length-prefix approach to indicate message size, or implementing timeouts and retires for robustness.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

**Ans.** Since TCP treats data as a continuous stream rather than discrete messages, a networked shell must define clear boundaries for each command. This can be done using delimiters (e.g., `\n ` or a special marker), length-prefixed messages, or a structured protocol (like JSON with a clear end marker). If boundaries aren't handled properly, commands may be concatenated or split unpredictably, leading to corruption or incorrect execution.

3. Describe the general differences between stateful and stateless protocols.

**Ans.** 

* **Stateful protocols ->** They keep track of previous interactions, allowing for session continuity (e.g., TCP, HTTPS with cookies). They require more resources but provide richer functionality.
* **Stateless protocols ->** They treat each request independently, with no memory of prior interactions (e.g., HTTP without cookies, UDP). They are simpler and scale better but require clients to resent context if needed.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

**Ans.** UDP is useful when speed matters more than reliability. It's great for real-time applications like video streaming, VoIP, and gaming, where occasional data loss is acceptable but low latency is crucial. It also works well for simple request-response models (e.g., DNS) where retransmission is handled at the application level.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

**Ans.** The OS provides sockets, which act as an abstraction for network communication. Applications use system calls like `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `send()`, and `recv()` to estabilish connections and exchange data over protocols like TCP and UDP.
