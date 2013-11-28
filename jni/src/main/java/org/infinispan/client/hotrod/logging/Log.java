package org.infinispan.client.hotrod.logging;

//import org.infinispan.client.hotrod.exceptions.HotRodClientException;
//import org.infinispan.client.hotrod.impl.transport.tcp.TcpTransport;
//import org.infinispan.commons.CacheConfigurationException;
import org.jboss.logging.BasicLogger;
import org.jboss.logging.Cause;
import org.jboss.logging.LogMessage;
import org.jboss.logging.Message;
import org.jboss.logging.MessageLogger;

import java.io.IOException;
import java.net.SocketAddress;
import java.util.Set;

import static org.jboss.logging.Logger.Level.*;

/**
 * Log abstraction for the hot rod client. For this module, message ids ranging from 4001 to 5000
 * inclusively have been reserved.
 *
 * @author Galder Zamarreño
 * @since 5.0
 */
@MessageLogger(projectCode = "ISPN")
public interface Log extends BasicLogger {

}
